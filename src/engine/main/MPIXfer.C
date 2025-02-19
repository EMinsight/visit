// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#ifdef PARALLEL
#include <cstdlib>
#include <MPIXfer.h>
#include <avtParallel.h>
#include <BufferConnection.h>
#include <AttributeSubject.h>
#include <DebugStream.h>
#include <TimingsManager.h>

#include <visitstream.h>
#include <visit-config.h>

#if defined(_WIN32)
#include <windows.h> // needed for Win32 SleepEx()
#endif

#if defined(HAVE_SELECT) && defined(VISIT_USE_NOSPIN_BCAST)
int MPIXfer::nanoSecsOfSleeps = 50000000; // 1/20th of a second
#else
int MPIXfer::nanoSecsOfSleeps = 0;        // Use MPI_Bcast
#endif
int MPIXfer::secsOfSpinBeforeSleeps = 5;  // 5 seconds
void (*MPIXfer::workerProcessInstruction)() = NULL;
const int UI_BCAST_TAG = GetUniqueStaticMessageTag();

// ****************************************************************************
// Method: MPIXfer::MPIXfer
//
// Purpose: 
//   Constructor for the MPIXfer class.
//
// Arguments:
//
// Returns:    
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Thu Jul 13 11:06:54 PDT 2000
//
// Modifications:
//   
// ****************************************************************************

MPIXfer::MPIXfer() : Xfer()
{
    enableReadHeader = true;
    readsSinceReadHeaderDisabled = 0;
}

// ****************************************************************************
// Method: MPIXfer::~MPIXfer
//
// Purpose: 
//   Destructor for the MPIXfer class.
//
// Arguments:
//
// Returns:    
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Thu Jul 13 11:07:16 PDT 2000
//
// Modifications:
//   
// ****************************************************************************

MPIXfer::~MPIXfer()
{
}

// ****************************************************************************
// Method: MPIXfer::SetEnableReadHeader
//
// Purpose: 
//   Sets an internal flag that helps to determine the value returned by the
//   ReadHeader method.
//
// Arguments:
//   val : The value set into the enableReadHeader flag.
//
// Programmer: Brad Whitlock
// Creation:   Fri Mar 16 10:56:19 PDT 2001
//
// Modifications:
//   
// ****************************************************************************

void
MPIXfer::SetEnableReadHeader(bool val)
{
    enableReadHeader = val;
    if(!enableReadHeader)
        readsSinceReadHeaderDisabled = 0;
}

// ****************************************************************************
// Method: MPIXfer::ReadHeader
//
// Purpose: 
//   Reads the header information for a message.
//
// Programmer: Brad Whitlock
// Creation:   Fri Mar 16 10:54:07 PDT 2001
//
// Modifications:
//   
// ****************************************************************************

bool
MPIXfer::ReadHeader()
{
    bool retval;

    if(enableReadHeader)
        retval = Xfer::ReadHeader();
    else
    {
        retval = (readsSinceReadHeaderDisabled == 0);
        ++readsSinceReadHeaderDisabled;            
    }

    return retval;
}


// ****************************************************************************
//  Method:  Xfer::SendInterruption
//
//  Purpose:
//    Send an interruption message to the remote connection.
//
//  Programmer:  Jeremy Meredith
//  Creation:    September 20, 2001
//
//  Modifications:
//
//    Mark C. Miller, Thu Jun 10 09:08:18 PDT 2004
//    Added arg for interrupt message tag
//
//    Mark C. Miller, Mon Jan 22 22:09:01 PST 2007
//    Changed MPI_COMM_WORLD to VISIT_MPI_COMM
//
//   Dave Pugmire, Fri Jan 14 11:31:41 EST 2011
//   Use MPI_Waitall instead. Also, cleaned up a memory leak.
//
// ****************************************************************************

void
MPIXfer::SendInterruption(int mpiInterruptTag)
{
    if (PAR_UIProcess())
    {
        // Do a nonblocking send to all processes to do it quickly
        int size = PAR_Size();
        unsigned char buf[1] = {255};
        MPI_Request *request = new MPI_Request[size-1];
        for (int i=1; i<size; i++)
        {
            MPI_Isend(buf, 1, MPI_UNSIGNED_CHAR, i, mpiInterruptTag, VISIT_MPI_COMM, &request[i-1]);
        }

        // Then wait for them all to read the command
        MPI_Status *status = new MPI_Status[size-1];
        MPI_Waitall(size-1, request, status);

        delete [] request;
        delete [] status;
    }
}


// ****************************************************************************
// Method: MPIXfer::Process
//
// Purpose: 
//   Reads the MPIXfer object's input connection and makes objects
//   update themselves when there are complete messages. In addition,
//   complete messages are broadcast to other processes on the
//   MPI world communicator.
//
// Programmer: Brad Whitlock
// Creation:   Thu Jul 13 10:20:48 PDT 2000
//
// Modifications:
//    Jeremy Meredith, Fri Sep 21 14:31:22 PDT 2001
//    Added use of buffered input.
//
//    Jeremy Meredith, Tue Mar  4 13:10:25 PST 2003
//    Used the length from the new buffer.  Multiple non-blocking messages
//    mess up MPIXfer if we don't keep track of the length for each message.
//
//    Jeremy Meredith, Thu Oct  7 14:09:10 PDT 2004
//    Added callback so the manager process could tell the workers they
//    are about to receive data.  This was needed for running inside a
//    parallel simulation because worker processes need some way to know
//    that the next command coming is visit-specific.
//
//    Mark C. Miller, Mon Jan 22 22:09:01 PST 2007
//    Changed MPI_COMM_WORLD to VISIT_MPI_COMM
//
//    Brad Whitlock, Fri Jun 19 09:05:31 PDT 2009
//    I rewrote the code for sending the command to other processors.
//    Instead of sending 1..N 1K size messages, with N being more common, we
//    now send 1 size message and then we send the entire command at once.
//
//    Brad Whitlock, Tue Nov  3 10:58:50 PST 2009
//    I removed one of the calls to workerprocesscallback since it was incorrect
//    to have it.
//
//    Brad Whitlock, Wed Oct 15 17:59:03 PDT 2014
//    Turn msgLength back to an int. We're communicating it to MPI after all 
//    as an int.
//
// ****************************************************************************

void
MPIXfer::Process()
{
    ReadPendingMessages();

    // While there are complete messages, read and process them.
    while(bufferedInput.Size() > 0)
    {
        int curOpcode;
        int curLength;
        bufferedInput.ReadInt(&curOpcode);
        bufferedInput.ReadInt(&curLength);

        if(subjectList[curOpcode])
        {
            if(PAR_UIProcess())
            {
                size_t i = 0;
                int msgLength = curLength + int(sizeof(int)*2);

#ifdef VISIT_BLUE_GENE_P
                // Make the buffer be 32-byte aligned
                unsigned char *buf = 0;
                posix_memalign((void **)&buf, 32, msgLength);
#else
                unsigned char *buf = (unsigned char*)malloc(msgLength * sizeof(unsigned char));
#endif
                unsigned char *tmp = buf;

                // Add the message's opcode and length into the buffer
                // that we'll broadcast to other processes so their
                // Xfer objects know what's up.
                unsigned char *cptr = (unsigned char *)&curOpcode;
                for(i = 0; i < sizeof(int); ++i)
                    *tmp++ = *cptr++;
                cptr = (unsigned char *)&curLength;
                for(i = 0; i < sizeof(int); ++i)
                    *tmp++ = *cptr++;

                // Transcribe the message into a buffer that we'll
                // communicate to other processes and also a new
                // temporary connection that we'll feed to the interested
                // object.
                BufferConnection newInput;
                unsigned char c;
                for(i = curLength; i > 0; --i)
                {
                    // Read a character from the input connection.
                    bufferedInput.ReadChar(&c);

                    *tmp++ = c;
                    newInput.WriteChar(c);
                }

                // Send the current opcode and message length. Note that we
                // use VisIt_MPI_Bcast for this call since we need to match
                // what's happening in PAR_EventLoop in the engine. This version
                // of bcast allows engines to reduce their activity instead of
                // using a spin-wait bcast.
                if (workerProcessInstruction)
                    workerProcessInstruction();

                VisIt_MPI_Bcast((void *)&msgLength, 1, MPI_INT,
                              0, VISIT_MPI_COMM);

                // Use regular bcast since the previous call to bcast will 
                // already have gotten the attention of the other processors.
                MPI_Bcast((void *)buf, msgLength, MPI_UNSIGNED_CHAR,
                           0, VISIT_MPI_COMM);
                free(buf);

                // Read the object from the newInput into its local copy.
                subjectList[curOpcode]->Read(newInput);
            }
            else
            {
                // Non-UI Processes can read the object from *input.
                subjectList[curOpcode]->Read(bufferedInput);
            }

            // Indicate that we want Xfer to ignore update messages if
            // it gets them while processing the Notify.
            SetUpdate(false);
            subjectList[curOpcode]->Notify();
        }
    }
}


// ****************************************************************************
// Method: MPIXfer::Update
//
// Purpose: 
//   Action performed when a subject changes
//
// Arguments:
//
// Returns:    
//
// Note:       This is the same as MPI::Update except that
//             only the UI process actually sends anything.
//
// Programmer: Jeremy Meredith
// Creation:   September 21, 2000
//
// Modifications:
//
// ****************************************************************************

void
MPIXfer::Update(Subject *TheChangedSubject)
{
    // We only do this until we know how to merge replies....
    if (!PAR_UIProcess())
        return;

    if (output == NULL)
        return;

    AttributeSubject *subject = (AttributeSubject *)TheChangedSubject;

    // Write out the subject's guido and message size.
    output->WriteInt(subject->GetGuido());
    output->WriteInt(subject->CalculateMessageSize(*output));

    // Write the things about the subject that have changed onto the
    // output connection and flush it out to make sure it's sent.
    subject->Write(*output);
    output->Flush();
}

// ****************************************************************************
//  Method:  MPIXfer::SetWorkerProcessInstructionCallback
//
//  Purpose:
//    Sets the callback for the manager process to tell the workers
//    they are about to receive data to process.
//
//  Arguments:
//    spi        the callback function
//
//  Programmer:  Jeremy Meredith
//  Creation:    October  7, 2004
//
// ****************************************************************************
void
MPIXfer::SetWorkerProcessInstructionCallback(void (*spi)())
{
    workerProcessInstruction = spi;
}

// ****************************************************************************
//  Function: VisIt_MPI_Bcast 
//
//  Purpose: A smarter broadcast that gives VisIt control over polling
//  behavior. MPI's Bcast method can wind up doing a 'spin-wait' eating cpu
//  resources. Our implementation, here, is both similar and different to
//  that.
//
//  In our Bcast method, all processors (except for root) start by posting a
//  non-blocking receive. A non-blocking receive can be tested for completion
//  using MPI_Test. All processors (again except for root), enter a polling
//  loop calling MPI_Test to check to see if the receive completed. For the
//  first several seconds of inactivity, they poll as fast as they can (this
//  is equivalent to MPI's spin-wait behavior). However, after not too much
//  time, they begin to introduce delays, using nanosleep, into this polling
//  loop. The delays substantially reduce the load on the cpu.
//  
//  The broadcast itself is initiated at the root and proceeds in tree-like
//  fashion. The root sends a message to the highest power of 2 ranked 
//  processor that is less than the communicator size. As that processor
//  completes its recieve, it exits from its polling loop and enters the
//  're-send to other processors' phase of the broadcast. It begins to send
//  messages to other processors and, simultaneously, the root also continues
//  to send messages to other processors. More processors complete their
//  recieves and then send the message on to still other processors. This
//  continues in tree-like fashion based on the MPI rank of the processors
//  until everyone has finished sending messages to those they are responsible
//  for. As the process continues, all odd-numbered processors only ever
//  execute a receive. All even numbered processors execute the receive and
//  then a variable number of sends depending on their rank relative to the
//  next closest power-of-2.
//
//  In the case of a 13 processor run, this is how the broadcast phase would
//  proceed...
//
//  1) P0->P8
//  2) PO->P4                          P8->P12
//  3) P0->P2,         P4->P6,         P8->P10
//  4) P0->P1, P2->P3, P4->P5, P6->P7, P8->P9, P10->P11
//
//  The first thing each processor does is to compute who it will recieve
//  the message from. That is determined by zeroing the lowest-order '1' bit
//  in the binary expression of the processor's rank. The difference between
//  that processor and the executing processor is a power-of-2. The executing
//  processor then sends the message to all processors that are above it in
//  rank by all powers-of-2 between 1 and the difference less one power of 2. 
//
//  Programmer: Mark C. Miller 
//  Creation:   February 12, 2007 
//
//  Modifications:
//
//    Mark C. Miller, Wed Feb 14 14:36:11 PST 2007
//    Added class statics to control behavior and fall back to MPI's Bcast
//    when we specify 0 sleep time.
// ****************************************************************************
int
MPIXfer::VisIt_MPI_Bcast(void *buf, int count, MPI_Datatype datatype, int root,
    MPI_Comm comm)
{
    //
    // Fall back to MPI broadcast if zero sleep time is specified
    //
    if (nanoSecsOfSleeps <= 0)
    {
        static bool first = true;
        if (first) 
        {
            debug5 << "Using MPI's Bcast; not VisIt_MPI_Bcast" << endl;
        }
        first = false;

        MPI_Bcast(buf, count, datatype, root, comm);
        return 2;
    }
        
    int rank = PAR_Rank();
    int size = PAR_Size();
    MPI_Status mpiStatus;

    //
    // Make proc 0 the root if it isn't already
    //
    if (root != 0)
    {
        if (rank == root) // only original root does this
            MPI_Send(buf, count, datatype, 0, UI_BCAST_TAG, comm);
        if (rank == 0)    // only new root (zero) does this
            MPI_Recv(buf, count, datatype, root, UI_BCAST_TAG, comm, &mpiStatus);
        root = 0;         // everyone does this
    }

    //
    // Compute who the executing proc. will recieve its message from.
    //
    int srcProc = 0;
    for (int i = 0; i < 31; i++)
    {
        int mask = 0x00000001;
        int bit = (rank >> i) & mask;
        if (bit == 1)
        {
            int mask1 = ~(0x00000001 << i);
            srcProc = (rank & mask1);
            break;
        }
    }

    //
    // Polling Phase
    //
    if (rank != 0) 
    {
        //
        // Everyone posts a non-blocking recieve
        //
        MPI_Request bcastRecv;
        MPI_Irecv(buf, count, datatype, srcProc, UI_BCAST_TAG, comm, &bcastRecv);

        //
        // Main polling loop
        //
        double startedIdlingAt = TOA_THIS_LINE;
        int mpiFlag;
        bool first = true;
        while (true)
        {
            // non-blocking test for recv completion
            MPI_Test(&bcastRecv, &mpiFlag, &mpiStatus);
            if (mpiFlag == 1)
                break;

            //
            // Note: We could add logic here to deal with engine idle timeout
            // instead of using the alarm mechanism we currently use.
            //

            //
            // Insert nanosleeps into the polling loop as determined by
            // amount of time we've been sitting here in this loop
            //
            double idleTime = TOA_THIS_LINE - startedIdlingAt;
            if (idleTime > secsOfSpinBeforeSleeps)
            {
                if (first) 
                {
                    debug5 << "VisIt_MPI_Bcast started using " << nanoSecsOfSleeps / 1.0e9
                           << " seconds of nanosleep" << endl;
                }
                first = false;
#if defined(_WIN32)
                SleepEx((DWORD)(nanoSecsOfSleeps/1e6), false);
#else
                struct timespec ts = {0, nanoSecsOfSleeps};
                nanosleep(&ts, 0);
#endif
            }
        }
    }

    //
    // Send on to other processors phase
    //

    //
    // Determine highest rank proc above the executing proc
    // that it is responsible to send a message to. 
    //
    int deltaProc = (rank - srcProc) >> 1;
    if (rank == 0)
    {
        deltaProc = 1;
        while ((deltaProc << 1) < PAR_Size())
            deltaProc = deltaProc << 1;
    }

    //
    // Send message to other procs the executing proc is responsible for 
    //
    while (deltaProc > 0)
    {
        if (rank + deltaProc < size)
            MPI_Send(buf, count, datatype, rank + deltaProc, UI_BCAST_TAG, comm);
        deltaProc = deltaProc >> 1;
    }

    return 0;
}
#endif
