// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <QvisMessageWindow.h>
#include <Observer.h>
#include <MessageAttributes.h>

#include <TimingsManager.h> // for DELTA_TOA_THIS_LINE
#include <UnicodeHelper.h>

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTextEdit>

// ****************************************************************************
// Method: QvisMessageWindow::QvisMessageWindow
//
// Purpose: 
//   This is the constructor for the QvisMessageWindow class. Note
//   that it creates its widgets right away. This is one of the few
//   windows that needs to do this.
//
// Arguments:
//   msgAttr : A pointer to the MessageAttributes that this window will
//             observe.
//   captionString : The caption that is in the window's window decor.
//
// Programmer: Brad Whitlock
// Creation:   Wed Aug 30 18:10:10 PST 2000
//
// Modifications:
//   Brad Whitlock, Mon Sep 24 17:10:32 PST 2001
//   I stuck in code using a real message to set the minimum size of the widget
//   based on the font.
//
//   Brad Whitlock, Mon May 12 13:00:03 PST 2003
//   I made the text read only.
//
//   Brad Whitlock, Fri Jan 18 16:11:07 PST 2008
//   Added preserveInformation, doHide slot, and made the window wider&taller by default.
//
//   Brad Whitlock, Tue Apr  8 09:27:26 PDT 2008
//   Support for internationalization.
//
//   Brad Whitlock, Fri May 30 14:28:01 PDT 2008
//   Qt 4.
//
//   Tom Fogal, Sun Jan 24 17:05:48 MST 2010
//   Patch from Andreas Kloeckner to set Qt window role.
//
//   Eric Brugger, Tue Aug 24 13:30:28 PDT 2010
//   Added a control to enable/disable the popping up of warning messages.
//
//   Kathleen Biagas, Wed Apr 6, 2022
//   Fix QT_VERSION test to use Qt's QT_VERSION_CHECK.
//
// ****************************************************************************

QvisMessageWindow::QvisMessageWindow(MessageAttributes *msgAttr,
    const QString &captionString) : QvisWindowBase(captionString, Qt::Dialog),
    Observer(msgAttr)
{
    setWindowRole("message");

    preserveInformation = false;
    enableWarningPopups = true;

    // Create the central widget and the top layout.
    QWidget *central = new QWidget( this );
    setCentralWidget( central );
    QVBoxLayout *topLayout = new QVBoxLayout(central);
    topLayout->setContentsMargins(10,10,10,10);

    // Create a multi line edit to display the message text.
    messageText = new QTextEdit(central);
    messageText->setWordWrapMode(QTextOption::WordWrap);
    messageText->setReadOnly(true);
    QString cm("Closed the compute engine on host sunburn.llnl.gov.  ");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int w = fontMetrics().horizontalAdvance(cm);
#else
    int w = fontMetrics().width(cm);
#endif
    messageText->setMinimumWidth(3 * w / 2);
    messageText->setMinimumHeight(8 * fontMetrics().lineSpacing());
    severityLabel = new QLabel(tr("Message"), central);
    severityLabel->setBuddy(messageText);
    QFont f("helvetica", 18);
    f.setBold(true);
    severityLabel->setFont(f);
    topLayout->addWidget(severityLabel);
    topLayout->addSpacing(10);
    topLayout->addWidget(messageText);
    topLayout->addSpacing(10);

    QHBoxLayout *buttonLayout = new QHBoxLayout(0);
    topLayout->addLayout(buttonLayout);

    // Create a button to hide the window.
    QPushButton *dismissButton = new QPushButton(tr("Dismiss"), central);
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(dismissButton);
    connect(dismissButton, SIGNAL(clicked()), this, SLOT(doHide()));
}

// *************************************************************************************
// Method: QvisMessageWindow::~QvisMessageWindow
//
// Purpose: 
//   Destructor for the QvisMessageWindow class.
//
// Programmer: Brad Whitlock
// Creation:   Wed Aug 30 18:12:09 PST 2000
//
// Modifications:
//   
// *************************************************************************************

QvisMessageWindow::~QvisMessageWindow()
{
    // nothing
}

// *************************************************************************************
// Method: QvisMessageWindow::SetEnableWarningPopups
//
// Purpose: 
//   Sets the enable warning popups flag.
//
// Programmer: Eric Brugger
// Creation:   Tue Aug 24 13:30:28 PDT 2010
//
// Modifications:
//   
// *************************************************************************************

void
QvisMessageWindow::SetEnableWarningPopups(bool val)
{
    enableWarningPopups = val;
}

// *************************************************************************************
// Method: QvisMessageWindow::Update
//
// Purpose: 
//   This method is called when the MessageAttributes object that this
//   window is watching changes. It's this window's responsibility to
//   write the message into the window and display it.
//
// Programmer: Brad Whitlock
// Creation:   Wed Aug 30 18:12:32 PST 2000
//
// Modifications:
//   Brad Whitlock, Tue May 20 15:07:59 PST 2003
//   I made it work with the regenerated MessageAttributes.
//
//   Brad Whitlock, Wed Sep 10 09:44:44 PDT 2003
//   I made the cursor get reset for error and warning messages.
//
//   Mark C. Miller Wed Apr 21 12:42:13 PDT 2004
//   I made it smarter about dealing with messages that occur close together in time.
//   Now, it will catenate them.
//
//   Mark C. Miller, Wed Jun 29 17:04:13 PDT 2005
//   I made it catenate new message *only* if existing message didn't already
//   contain text of new message.
//
//   Brad Whitlock, Thu May 11 15:01:21 PST 2006
//   Return if the message is ErrorClear.
//
//   Brad Whitlock, Fri Jan 18 14:34:55 PST 2008
//   Added Information, which is similar to Message but shows the window.
//   The Information message is shown until a new information, error, or
//   warning message comes in. Incoming "Message" messages do not overwrite
//   an Information message while the window is showing.
//
//   Brad Whitlock, Tue Apr 29 10:27:01 PDT 2008
//   Support for internationalization.
//
//   Eric Brugger, Tue Aug 24 13:30:28 PDT 2010
//   Added a control to enable/disable the popping up of warning messages.
//
// *************************************************************************************

void
QvisMessageWindow::Update(Subject *)
{
    MessageAttributes *ma = (MessageAttributes *)subject;

    MessageAttributes::Severity severity = ma->GetSeverity();
    if(severity == MessageAttributes::ErrorClear)
        return;

    double secondsSinceLastMessage = DELTA_TOA_THIS_LINE;
    QString msgText;
    if (secondsSinceLastMessage < 5.0)
    {
        MessageAttributes::Severity oldSeverity;
        QString oldSeverityLabel = severityLabel->text();
        if (oldSeverityLabel == tr("Error!"))
            oldSeverity = MessageAttributes::Error;
        else if (oldSeverityLabel == tr("Warning"))
            oldSeverity = MessageAttributes::Warning;
        else if (oldSeverityLabel == tr("Message"))
            oldSeverity = MessageAttributes::Message;
        else if (oldSeverityLabel == tr("Information"))
            oldSeverity = MessageAttributes::Information;
        else
            oldSeverity = MessageAttributes::Error;

        // If we're not in information mode, append the incoming messages
        if(!preserveInformation)
        {
            // set severity to whichever is worse
            if (oldSeverity < severity)
                severity = oldSeverity;

            // catenate new message onto old 
            msgText = messageText->toPlainText();
            QString newMsgText = MessageAttributes_GetText(*ma);
            if (msgText.indexOf(newMsgText) == -1)
            {
                msgText += "\n\n";
                msgText += tr("Shortly thereafter, the following occured...");
                msgText += "\n\n";
                msgText += newMsgText;
            }
        }
        else if(severity == MessageAttributes::Information)
        {
            msgText = MessageAttributes_GetText(*ma);
            preserveInformation = false;
        }
        else if((severity == MessageAttributes::Error ||
                 severity == MessageAttributes::Warning) &&
                 oldSeverity == MessageAttributes::Information)
        {
            // Incoming Error, Warnings may overwrite Information.
            msgText = MessageAttributes_GetText(*ma);
            preserveInformation = false;
        }
    }
    else
    {
        msgText = MessageAttributes_GetText(*ma);
        
        // Don't preserve information if a new information message is
        // coming in. Also let error, warning override the existing
        // information message.
        if(preserveInformation && 
           (severity == MessageAttributes::Error ||
            severity == MessageAttributes::Warning ||
            severity == MessageAttributes::Information))
        {
            preserveInformation = false;
        }
    }

    if(!preserveInformation)
    {
        // Set the severity label text.
        if(severity == MessageAttributes::Error)
        {
            show();
            qApp->beep();
            severityLabel->setText(tr("Error!"));
            RestoreCursor();
        }
        else if(severity == MessageAttributes::Warning)
        {
            if (enableWarningPopups)
                show();
            severityLabel->setText(tr("Warning"));
            RestoreCursor();
        }
        else if(severity == MessageAttributes::Message)
            severityLabel->setText(tr("Message"));
        else if(severity == MessageAttributes::Information)
        {
            show();
            severityLabel->setText(tr("Information"));
            RestoreCursor();
            preserveInformation = true;
        }

        // Set the message text.
        messageText->setText(msgText);
    }
}

// ****************************************************************************
// Method: QvisMessageWindow::doHide
//
// Purpose: 
//   Hides the window and turns off the preserveInformation mode.
//
// Arguments:
//
// Returns:    
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Fri Jan 18 15:30:48 PST 2008
//
// Modifications:
//   
// ****************************************************************************

void
QvisMessageWindow::doHide()
{
    preserveInformation = false;
    hide();
}

// ****************************************************************************
// Method: QvisMessageWindow::EnableWarningPopups
//
// Purpose: 
//   Sets the enable warning popups flag.
//
// Arguments:
//
// Programmer: Eric Brugger
// Creation:   Tue Aug 24 13:30:28 PDT 2010
//
// Modifications:
//   
// ****************************************************************************

void
QvisMessageWindow::EnableWarningPopups(bool val)
{
    enableWarningPopups = val;
}

