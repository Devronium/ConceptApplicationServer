#include "MAPIExPCH.h"
//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "ConceptMAPI.h"
#include <string.h>
#include "AnsiString.h"
#include "MAPIEx.h"
#include "NetMAPI.h"


INVOKE_CALL InvokePtr = 0;

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(CMAPIEx::PROP_MESSAGE_FLAGS)
    DEFINE_ECONSTANT(CMAPIEx::PROP_ENTRYID)
    DEFINE_ECONSTANT(CMAPIEx::MESSAGE_COLS)

    DEFINE_ECONSTANT(PR_BODY_HTML)
    DEFINE_ECONSTANT(STORE_HTML_OK)
    DEFINE_ECONSTANT(PR_SMTP_ADDRESS)
    DEFINE_ECONSTANT(PR_IPM_APPOINTMENT_ENTRYID)
    DEFINE_ECONSTANT(PR_IPM_CONTACT_ENTRYID)
    DEFINE_ECONSTANT(PR_IPM_JOURNAL_ENTRYID)
    DEFINE_ECONSTANT(PR_IPM_NOTE_ENTRYID)
    DEFINE_ECONSTANT(PR_IPM_TASK_ENTRYID)
    DEFINE_ECONSTANT(PR_IPM_DRAFTS_ENTRYID)

    DEFINE_ECONSTANT(CContactAddress::HOME)
    DEFINE_ECONSTANT(CContactAddress::BUSINESS)
    DEFINE_ECONSTANT(CContactAddress::OTHER)
    DEFINE_ECONSTANT(CContactAddress::MAX_ADDRESS_TYPES)

    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_DATA1)
    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_EMAIL1)
    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_EMAIL2)
    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_EMAIL3)
    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_FILE_AS)
    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_POSTAL_ADDRESS)
    DEFINE_ECONSTANT(CMAPIContact::OUTLOOK_DISPLAY_ADDRESS_HOME)

    DEFINE_ECONSTANT(CMAPIMessage::PROP_RECIPIENT_TYPE)
    DEFINE_ECONSTANT(CMAPIMessage::PROP_RECIPIENT_NAME)
    DEFINE_ECONSTANT(CMAPIMessage::PROP_RECIPIENT_EMAIL)
    DEFINE_ECONSTANT(CMAPIMessage::PROP_ADDR_TYPE)
    DEFINE_ECONSTANT(CMAPIMessage::PROP_ENTRYID)
    DEFINE_ECONSTANT(CMAPIMessage::RECIPIENT_COLS)
    //MAPIInit();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIInit, 0)

    RETURN_NUMBER(MAPIInit())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPITerm, 0)

    MAPITerm();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPILogin, 1)
    T_STRING_NULL(MAPILogin, 0)     // char*

    RETURN_NUMBER((long)MAPILogin(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPILogout, 1)
    T_NUMBER(MAPILogout, 0)     // CMAPIEx

    MAPILogout((CMAPIEx *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenMessageStore, 2)
    T_NUMBER(MAPIOpenMessageStore, 0)      // CMAPIEx
    T_STRING_NULL(MAPIOpenMessageStore, 1) // char*

    RETURN_NUMBER(MAPIOpenMessageStore((CMAPIEx *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
#define PROP(len_index)     char *value = new char[PARAM_INT(len_index) + 1]; value[PARAM_INT(len_index)] = 0;
#define RET(index)          SET_STRING(index, value); delete[] value;

#define PROP2(len_index)    char *value2 = new char[PARAM_INT(len_index) + 1]; value2[PARAM_INT(len_index)] = 0;
#define RET2(index)         SET_STRING(index, value2); delete[] value2;

CONCEPT_FUNCTION_IMPL(MAPIGetProfileName, 3)
    T_NUMBER(MAPIGetProfileName, 0)     // CMAPIEx
//T_STRING_NULL(1) // char*
    T_NUMBER(MAPIGetProfileName, 2)     // int

    PROP(2)
    MAPIGetProfileName((CMAPIEx *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1);

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenRootFolder, 1)
    T_NUMBER(MAPIOpenRootFolder, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenRootFolder((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenInbox, 1)
    T_NUMBER(MAPIOpenInbox, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenInbox((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenOutbox, 1)
    T_NUMBER(MAPIOpenOutbox, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenOutbox((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenSentItems, 1)
    T_NUMBER(MAPIOpenSentItems, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenSentItems((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenDeletedItems, 1)
    T_NUMBER(MAPIOpenDeletedItems, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenDeletedItems((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenContacts, 1)
    T_NUMBER(MAPIOpenContacts, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenContacts((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenDrafts, 1)
    T_NUMBER(MAPIOpenDrafts, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIOpenDrafts((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetHierarchy, 2)
    T_NUMBER(MAPIGetHierarchy, 0)     // CMAPIEx
    T_NUMBER(MAPIGetHierarchy, 1)     // LPMAPIFOLDER

    RETURN_NUMBER(MAPIGetHierarchy((CMAPIEx *)PARAM_INT(0), (LPMAPIFOLDER)PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetFolder, 1)
    T_NUMBER(MAPIGetFolder, 0)     // CMAPIEx

    RETURN_NUMBER((long)MAPIGetFolder((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetNextSubFolder, 3)
    T_NUMBER(MAPIGetNextSubFolder, 0)     // CMAPIEx
//T_STRING_NULL(1) // char*
    T_NUMBER(MAPIGetNextSubFolder, 2)     // int

    PROP(2)
    RETURN_NUMBER((long)MAPIGetNextSubFolder((CMAPIEx *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIOpenSubFolder, 3)
    T_NUMBER(MAPIOpenSubFolder, 0)      // CMAPIEx
    T_STRING_NULL(MAPIOpenSubFolder, 1) // char*
    T_NUMBER(MAPIOpenSubFolder, 2)      // LPMAPIFOLDER

    RETURN_NUMBER(MAPIOpenSubFolder((CMAPIEx *)PARAM_INT(0), PARAM(1), (LPMAPIFOLDER)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPICreateSubFolder, 3)
    T_NUMBER(MAPICreateSubFolder, 0)      // CMAPIEx
    T_STRING_NULL(MAPICreateSubFolder, 1) // char*
    T_NUMBER(MAPICreateSubFolder, 2)      // LPMAPIFOLDER

    RETURN_NUMBER((long)MAPICreateSubFolder((CMAPIEx *)PARAM_INT(0), PARAM(1), (LPMAPIFOLDER)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIDeleteSubFolderByName, 3)
    T_NUMBER(MAPIDeleteSubFolderByName, 0)      // CMAPIEx
    T_STRING_NULL(MAPIDeleteSubFolderByName, 1) // char*
    T_NUMBER(MAPIDeleteSubFolderByName, 2)      // LPMAPIFOLDER

    RETURN_NUMBER(MAPIDeleteSubFolderByName((CMAPIEx *)PARAM_INT(0), PARAM(1), (LPMAPIFOLDER)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIDeleteSubFolder, 3)
    T_NUMBER(MAPIDeleteSubFolder, 0)     // CMAPIEx
    T_NUMBER(MAPIDeleteSubFolder, 1)     // LPMAPIFOLDER
    T_NUMBER(MAPIDeleteSubFolder, 2)     // LPMAPIFOLDER

    RETURN_NUMBER(MAPIDeleteSubFolder((CMAPIEx *)PARAM_INT(0), (LPMAPIFOLDER)PARAM_INT(1), (LPMAPIFOLDER)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIReleaseFolder, 1)
    T_NUMBER(MAPIReleaseFolder, 0)     // LPMAPIFOLDER

    MAPIReleaseFolder((LPMAPIFOLDER)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetContents, 2)
    T_NUMBER(MAPIGetContents, 0)     // CMAPIEx
    T_NUMBER(MAPIGetContents, 1)     // LPMAPIFOLDER

    RETURN_NUMBER(MAPIGetContents((CMAPIEx *)PARAM_INT(0), (LPMAPIFOLDER)PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetRowCount, 1)
    T_NUMBER(MAPIGetRowCount, 0)     // CMAPIEx

    RETURN_NUMBER(MAPIGetRowCount((CMAPIEx *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPISortContents, 3)
    T_NUMBER(MAPISortContents, 0)     // CMAPIEx
    T_NUMBER(MAPISortContents, 1)     // int
    T_NUMBER(MAPISortContents, 2)     // int

    RETURN_NUMBER(MAPISortContents((CMAPIEx *)PARAM_INT(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetNextMessage, 3)
    T_NUMBER(MAPIGetNextMessage, 0)     // CMAPIEx
    T_NUMBER(MAPIGetNextMessage, 1)     // CMAPIMessage
    T_NUMBER(MAPIGetNextMessage, 2)     // int

    CMAPIMessage * ca = (CMAPIMessage *)PARAM_INT(1);
    RETURN_NUMBER(MAPIGetNextMessage((CMAPIEx *)PARAM_INT(0), ca, (int)PARAM(2)))
    SET_NUMBER(1, (long)ca);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIGetNextContact, 2)
    T_NUMBER(MAPIGetNextContact, 0)     // CMAPIEx
    T_NUMBER(MAPIGetNextContact, 1)     // CMAPIContact

    CMAPIContact * ca = (CMAPIContact *)PARAM_INT(1);
    RETURN_NUMBER(MAPIGetNextContact((CMAPIEx *)PARAM_INT(0), ca))
    SET_NUMBER(1, (long)ca);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIDeleteMessage, 3)
    T_NUMBER(MAPIDeleteMessage, 0)     // CMAPIEx
    T_NUMBER(MAPIDeleteMessage, 1)     // CMAPIMessage
    T_NUMBER(MAPIDeleteMessage, 2)     // LPMAPIFOLDER

    RETURN_NUMBER(MAPIDeleteMessage((CMAPIEx *)PARAM_INT(0), (CMAPIMessage *)PARAM_INT(1), (LPMAPIFOLDER)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPICopyMessage, 4)
    T_NUMBER(MAPICopyMessage, 0)     // CMAPIEx
    T_NUMBER(MAPICopyMessage, 1)     // CMAPIMessage
    T_NUMBER(MAPICopyMessage, 2)     // LPMAPIFOLDER
    T_NUMBER(MAPICopyMessage, 3)     // LPMAPIFOLDER

    RETURN_NUMBER(MAPICopyMessage((CMAPIEx *)PARAM_INT(0), (CMAPIMessage *)PARAM_INT(1), (LPMAPIFOLDER)PARAM_INT(2), (LPMAPIFOLDER)PARAM_INT(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MAPIMoveMessage, 4)
    T_NUMBER(MAPIMoveMessage, 0)     // CMAPIEx
    T_NUMBER(MAPIMoveMessage, 1)     // CMAPIMessage
    T_NUMBER(MAPIMoveMessage, 2)     // LPMAPIFOLDER
    T_NUMBER(MAPIMoveMessage, 3)     // LPMAPIFOLDER

    RETURN_NUMBER(MAPIMoveMessage((CMAPIEx *)PARAM_INT(0), (CMAPIMessage *)PARAM_INT(1), (LPMAPIFOLDER)PARAM_INT(2), (LPMAPIFOLDER)PARAM_INT(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageClose, 1)
    T_NUMBER(MessageClose, 0)     // CMAPIMessage

    MessageClose((CMAPIMessage *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageCreate, 3)
    T_NUMBER(MessageCreate, 0)     // CMAPIEx
    T_NUMBER(MessageCreate, 1)     // CMAPIMessage
    T_NUMBER(MessageCreate, 2)     // int

    CMAPIMessage * ca = (CMAPIMessage *)PARAM_INT(1);
    RETURN_NUMBER(MessageCreate((CMAPIEx *)PARAM_INT(0), ca, (int)PARAM(2)))
    SET_NUMBER(1, (long)ca);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageShowForm, 2)
    T_NUMBER(MessageShowForm, 0)     // CMAPIEx
    T_NUMBER(MessageShowForm, 1)     // CMAPIMessage

    RETURN_NUMBER(MessageShowForm((CMAPIEx *)PARAM_INT(0), (CMAPIMessage *)PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSave, 2)
    T_NUMBER(MessageSave, 0)     // CMAPIMessage
    T_NUMBER(MessageSave, 1)     // int

    RETURN_NUMBER(MessageSave((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSend, 1)
    T_NUMBER(MessageSend, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageSend((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageIsUnread, 1)
    T_NUMBER(MessageIsUnread, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageIsUnread((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageMarkAsRead, 2)
    T_NUMBER(MessageMarkAsRead, 0)     // CMAPIMessage
    T_NUMBER(MessageMarkAsRead, 1)     // int

    RETURN_NUMBER(MessageMarkAsRead((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetHeader, 3)
    T_NUMBER(MessageGetHeader, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetHeader, 2)     // int

    PROP(2)
    RETURN_NUMBER(MessageGetHeader((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetSenderName, 3)
    T_NUMBER(MessageGetSenderName, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetSenderName, 2)     // int

    PROP(2)
    MessageGetSenderName((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetSenderEmail, 3)
    T_NUMBER(MessageGetSenderEmail, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetSenderEmail, 2)     // int

    PROP(2)
    MessageGetSenderEmail((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetSubject, 3)
    T_NUMBER(MessageGetSubject, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetSubject, 2)     // int

    PROP(2)
    MessageGetSubject((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetBodySize, 1)
    T_NUMBER(MessageGetBodySize, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageGetBodySize((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetBody, 3)
    T_NUMBER(MessageGetBody, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetBody, 2)     // int

    PROP(2)
    MessageGetBody((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetRTFSize, 1)
    T_NUMBER(MessageGetRTFSize, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageGetRTFSize((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetRTF, 3)
    T_NUMBER(MessageGetRTF, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetRTF, 2)     // int

    PROP(2)
    MessageGetRTF((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetReceivedTime, 7)
    T_NUMBER(MessageGetReceivedTime, 0)     // CMAPIMessage

// ... parameter 1 is by reference (int&)
    int local_parameter_1;
// ... parameter 2 is by reference (int&)
    int local_parameter_2;
// ... parameter 3 is by reference (int&)
    int local_parameter_3;
// ... parameter 4 is by reference (int&)
    int local_parameter_4;
// ... parameter 5 is by reference (int&)
    int local_parameter_5;
// ... parameter 6 is by reference (int&)
    int local_parameter_6;

    RETURN_NUMBER(MessageGetReceivedTime((CMAPIMessage *)PARAM_INT(0), local_parameter_1, local_parameter_2, local_parameter_3, local_parameter_4, local_parameter_5, local_parameter_6))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetReceivedTimeString, 4)
    T_NUMBER(MessageGetReceivedTimeString, 0)      // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetReceivedTimeString, 2)      // int
    T_STRING_NULL(MessageGetReceivedTimeString, 3) // char*

    PROP(2)
    RETURN_NUMBER(MessageGetReceivedTimeString((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2), PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetSubmitTime, 7)
    T_NUMBER(MessageGetSubmitTime, 0)     // CMAPIMessage

// ... parameter 1 is by reference (int&)
    int local_parameter_1;
// ... parameter 2 is by reference (int&)
    int local_parameter_2;
// ... parameter 3 is by reference (int&)
    int local_parameter_3;
// ... parameter 4 is by reference (int&)
    int local_parameter_4;
// ... parameter 5 is by reference (int&)
    int local_parameter_5;
// ... parameter 6 is by reference (int&)
    int local_parameter_6;

    RETURN_NUMBER(MessageGetSubmitTime((CMAPIMessage *)PARAM_INT(0), local_parameter_1, local_parameter_2, local_parameter_3, local_parameter_4, local_parameter_5, local_parameter_6))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetSubmitTimeString, 4)
    T_NUMBER(MessageGetSubmitTimeString, 0)      // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetSubmitTimeString, 2)      // int
    T_STRING_NULL(MessageGetSubmitTimeString, 3) // char*

    PROP(2)
    RETURN_NUMBER(MessageGetSubmitTimeString((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2), PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetTo, 3)
    T_NUMBER(MessageGetTo, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetTo, 2)     // int

    PROP(2)
    RETURN_NUMBER(MessageGetTo((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetCC, 3)
    T_NUMBER(MessageGetCC, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetCC, 2)     // int

    PROP(2)
    RETURN_NUMBER(MessageGetCC((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetBCC, 3)
    T_NUMBER(MessageGetBCC, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetBCC, 2)     // int

    PROP(2)
    RETURN_NUMBER(MessageGetBCC((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetSensitivity, 1)
    T_NUMBER(MessageGetSensitivity, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageGetSensitivity((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetNamedProperty, 4)
    T_NUMBER(MessageGetNamedProperty, 0)      // CMAPIMessage
    T_STRING_NULL(MessageGetNamedProperty, 1) // char*
//T_STRING_NULL(2) // char*
    T_NUMBER(MessageGetNamedProperty, 3)      // int

    PROP(3)
    RETURN_NUMBER(MessageGetNamedProperty((CMAPIMessage *)PARAM_INT(0), PARAM(1), value, (int)PARAM(3)))
    RET(2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetMessageFlags, 1)
    T_NUMBER(MessageGetMessageFlags, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageGetMessageFlags((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetRecipients, 1)
    T_NUMBER(MessageGetRecipients, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageGetRecipients((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetNextRecipient, 6)
    T_NUMBER(MessageGetNextRecipient, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetNextRecipient, 2)     // int
//T_STRING_NULL(3) // char*
    T_NUMBER(MessageGetNextRecipient, 4)     // int

// ... parameter 5 is by reference (int&)
    int local_parameter_5;

    PROP(2)
    PROP2(4)
    RETURN_NUMBER(MessageGetNextRecipient((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2), value2, (int)PARAM(4), local_parameter_5))
    SET_NUMBER(5, (long)local_parameter_5)
    RET(1)
    RET2(3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetAttachmentCount, 1)
    T_NUMBER(MessageGetAttachmentCount, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageGetAttachmentCount((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageGetAttachmentName, 4)
    T_NUMBER(MessageGetAttachmentName, 0)     // CMAPIMessage
//T_STRING_NULL(1) // char*
    T_NUMBER(MessageGetAttachmentName, 2)     // int
    T_NUMBER(MessageGetAttachmentName, 3)     // int

    PROP(2)
    RETURN_NUMBER(MessageGetAttachmentName((CMAPIMessage *)PARAM_INT(0), value, (int)PARAM(2), (int)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSaveAttachment, 3)
    T_NUMBER(MessageSaveAttachment, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSaveAttachment, 1) // char*
    T_NUMBER(MessageSaveAttachment, 2)      // int

    RETURN_NUMBER(MessageSaveAttachment((CMAPIMessage *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageDeleteAttachment, 2)
    T_NUMBER(MessageDeleteAttachment, 0)     // CMAPIMessage
    T_NUMBER(MessageDeleteAttachment, 1)     // int

    RETURN_NUMBER(MessageDeleteAttachment((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetMessageStatus, 2)
    T_NUMBER(MessageSetMessageStatus, 0)     // CMAPIMessage
    T_NUMBER(MessageSetMessageStatus, 1)     // int

    RETURN_NUMBER(MessageSetMessageStatus((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageAddRecipient, 3)
    T_NUMBER(MessageAddRecipient, 0)      // CMAPIMessage
    T_STRING_NULL(MessageAddRecipient, 1) // char*
    T_NUMBER(MessageAddRecipient, 2)      // int

    RETURN_NUMBER(MessageAddRecipient((CMAPIMessage *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetSubject, 2)
    T_NUMBER(MessageSetSubject, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSetSubject, 1) // char*

    MessageSetSubject((CMAPIMessage *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetSenderName, 2)
    T_NUMBER(MessageSetSenderName, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSetSenderName, 1) // char*

    MessageSetSenderName((CMAPIMessage *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetSenderEmail, 2)
    T_NUMBER(MessageSetSenderEmail, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSetSenderEmail, 1) // char*

    MessageSetSenderEmail((CMAPIMessage *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetBody, 2)
    T_NUMBER(MessageSetBody, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSetBody, 1) // char*

    MessageSetBody((CMAPIMessage *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetRTF, 2)
    T_NUMBER(MessageSetRTF, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSetRTF, 1) // char*

    MessageSetRTF((CMAPIMessage *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageAddAttachment, 3)
    T_NUMBER(MessageAddAttachment, 0)      // CMAPIMessage
    T_STRING_NULL(MessageAddAttachment, 1) // char*
    T_STRING_NULL(MessageAddAttachment, 2) // char*

    RETURN_NUMBER(MessageAddAttachment((CMAPIMessage *)PARAM_INT(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetReadReceipt, 3)
    T_NUMBER(MessageSetReadReceipt, 0)      // CMAPIMessage
    T_NUMBER(MessageSetReadReceipt, 1)      // int
    T_STRING_NULL(MessageSetReadReceipt, 2) // char*

    RETURN_NUMBER(MessageSetReadReceipt((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetDeliveryReceipt, 2)
    T_NUMBER(MessageSetDeliveryReceipt, 0)     // CMAPIMessage
    T_NUMBER(MessageSetDeliveryReceipt, 1)     // int

    RETURN_NUMBER(MessageSetDeliveryReceipt((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageMarkAsPrivate, 1)
    T_NUMBER(MessageMarkAsPrivate, 0)     // CMAPIMessage

    RETURN_NUMBER(MessageMarkAsPrivate((CMAPIMessage *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetSensitivity, 2)
    T_NUMBER(MessageSetSensitivity, 0)     // CMAPIMessage
    T_NUMBER(MessageSetSensitivity, 1)     // int

    RETURN_NUMBER(MessageSetSensitivity((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetNamedProperty, 4)
    T_NUMBER(MessageSetNamedProperty, 0)      // CMAPIMessage
    T_STRING_NULL(MessageSetNamedProperty, 1) // char*
    T_STRING_NULL(MessageSetNamedProperty, 2) // char*
    T_NUMBER(MessageSetNamedProperty, 3)      // int

    RETURN_NUMBER(MessageSetNamedProperty((CMAPIMessage *)PARAM_INT(0), PARAM(1), PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MessageSetMessageFlags, 2)
    T_NUMBER(MessageSetMessageFlags, 0)     // CMAPIMessage
    T_NUMBER(MessageSetMessageFlags, 1)     // int

    RETURN_NUMBER(MessageSetMessageFlags((CMAPIMessage *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactClose, 1)
    T_NUMBER(ContactClose, 0)     // CMAPIContact

    ContactClose((CMAPIContact *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSave, 2)
    T_NUMBER(ContactSave, 0)     // CMAPIContact
    T_NUMBER(ContactSave, 1)     // int

    RETURN_NUMBER(ContactSave((CMAPIContact *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetPropertyString, 4)
    T_NUMBER(ContactGetPropertyString, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetPropertyString, 2)     // int
    T_NUMBER(ContactGetPropertyString, 3)     // ULONG

    PROP(2)
    RETURN_NUMBER(ContactGetPropertyString((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), (ULONG)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetName, 4)
    T_NUMBER(ContactGetName, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetName, 2)     // int
    T_NUMBER(ContactGetName, 3)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetName((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), (int)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetEmail, 4)
    T_NUMBER(ContactGetEmail, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetEmail, 2)     // int
    T_NUMBER(ContactGetEmail, 3)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetEmail((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), (int)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetEmailDisplayAs, 4)
    T_NUMBER(ContactGetEmailDisplayAs, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetEmailDisplayAs, 2)     // int
    T_NUMBER(ContactGetEmailDisplayAs, 3)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetEmailDisplayAs((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), (int)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetHomePage, 3)
    T_NUMBER(ContactGetHomePage, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetHomePage, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetHomePage((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetPhoneNumber, 4)
    T_NUMBER(ContactGetPhoneNumber, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetPhoneNumber, 2)     // int
    T_NUMBER(ContactGetPhoneNumber, 3)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetPhoneNumber((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), (int)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetAddress, 3)
    T_NUMBER(ContactGetAddress, 0)     // CMAPIContact
    T_NUMBER(ContactGetAddress, 1)     // CContactAddress
    T_NUMBER(ContactGetAddress, 2)     // int

    CContactAddress * ca = (CContactAddress *)PARAM_INT(1);
    RETURN_NUMBER(ContactGetAddress((CMAPIContact *)PARAM_INT(0), ca, (int)PARAM(2)))
    SET_NUMBER(1, (long)ca);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetPostalAddress, 3)
    T_NUMBER(ContactGetPostalAddress, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetPostalAddress, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetPostalAddress((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetNotesSize, 2)
    T_NUMBER(ContactGetNotesSize, 0)     // CMAPIContact
    T_NUMBER(ContactGetNotesSize, 1)     // int

    RETURN_NUMBER(ContactGetNotesSize((CMAPIContact *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetNotes, 4)
    T_NUMBER(ContactGetNotes, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetNotes, 2)     // int
    T_NUMBER(ContactGetNotes, 3)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetNotes((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), (int)PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetSensitivity, 1)
    T_NUMBER(ContactGetSensitivity, 0)     // CMAPIContact

    RETURN_NUMBER(ContactGetSensitivity((CMAPIContact *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetTitle, 3)
    T_NUMBER(ContactGetTitle, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetTitle, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetTitle((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetCompany, 3)
    T_NUMBER(ContactGetCompany, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetCompany, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetCompany((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetDisplayNamePrefix, 3)
    T_NUMBER(ContactGetDisplayNamePrefix, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetDisplayNamePrefix, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetDisplayNamePrefix((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetGeneration, 3)
    T_NUMBER(ContactGetGeneration, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetGeneration, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetGeneration((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetDepartment, 3)
    T_NUMBER(ContactGetDepartment, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetDepartment, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetDepartment((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetOffice, 3)
    T_NUMBER(ContactGetOffice, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetOffice, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetOffice((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetManagerName, 3)
    T_NUMBER(ContactGetManagerName, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetManagerName, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetManagerName((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetAssistantName, 3)
    T_NUMBER(ContactGetAssistantName, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetAssistantName, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetAssistantName((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetNickName, 3)
    T_NUMBER(ContactGetNickName, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetNickName, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetNickName((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetSpouseName, 3)
    T_NUMBER(ContactGetSpouseName, 0)     // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetSpouseName, 2)     // int

    PROP(2)
    RETURN_NUMBER(ContactGetSpouseName((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetBirthday, 4)
    T_NUMBER(ContactGetBirthday, 0)     // CMAPIContact

// ... parameter 1 is by reference (int&)
    int local_parameter_1;
// ... parameter 2 is by reference (int&)
    int local_parameter_2;
// ... parameter 3 is by reference (int&)
    int local_parameter_3;

    RETURN_NUMBER(ContactGetBirthday((CMAPIContact *)PARAM_INT(0), local_parameter_1, local_parameter_2, local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetBirthdayString, 4)
    T_NUMBER(ContactGetBirthdayString, 0)      // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetBirthdayString, 2)      // int
    T_STRING_NULL(ContactGetBirthdayString, 3) // char*

    PROP(2)
    RETURN_NUMBER(ContactGetBirthdayString((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetAnniversary, 4)
    T_NUMBER(ContactGetAnniversary, 0)     // CMAPIContact

// ... parameter 1 is by reference (int&)
    int local_parameter_1;
// ... parameter 2 is by reference (int&)
    int local_parameter_2;
// ... parameter 3 is by reference (int&)
    int local_parameter_3;

    RETURN_NUMBER(ContactGetAnniversary((CMAPIContact *)PARAM_INT(0), local_parameter_1, local_parameter_2, local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetAnniversaryString, 4)
    T_NUMBER(ContactGetAnniversaryString, 0)      // CMAPIContact
//T_STRING_NULL(1) // char*
    T_NUMBER(ContactGetAnniversaryString, 2)      // int
    T_STRING_NULL(ContactGetAnniversaryString, 3) // char*

    PROP(2)
    RETURN_NUMBER(ContactGetAnniversaryString((CMAPIContact *)PARAM_INT(0), value, (int)PARAM(2), PARAM(3)))
    RET(1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactGetNamedProperty, 4)
    T_NUMBER(ContactGetNamedProperty, 0)      // CMAPIContact
    T_STRING_NULL(ContactGetNamedProperty, 1) // char*
//T_STRING_NULL(2) // char*
    T_NUMBER(ContactGetNamedProperty, 3)      // int

    PROP(3)
    RETURN_NUMBER(ContactGetNamedProperty((CMAPIContact *)PARAM_INT(0), PARAM(1), value, (int)PARAM(3)))
    RET(2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetPropertyString, 3)
    T_NUMBER(ContactSetPropertyString, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetPropertyString, 1) // char*
    T_NUMBER(ContactSetPropertyString, 2)      // ULONG

    RETURN_NUMBER(ContactSetPropertyString((CMAPIContact *)PARAM_INT(0), PARAM(1), (ULONG)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetName, 3)
    T_NUMBER(ContactSetName, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetName, 1) // char*
    T_NUMBER(ContactSetName, 2)      // int

    RETURN_NUMBER(ContactSetName((CMAPIContact *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetEmail, 3)
    T_NUMBER(ContactSetEmail, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetEmail, 1) // char*
    T_NUMBER(ContactSetEmail, 2)      // int

    RETURN_NUMBER(ContactSetEmail((CMAPIContact *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetEmailDisplayAs, 3)
    T_NUMBER(ContactSetEmailDisplayAs, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetEmailDisplayAs, 1) // char*
    T_NUMBER(ContactSetEmailDisplayAs, 2)      // int

    RETURN_NUMBER(ContactSetEmailDisplayAs((CMAPIContact *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetHomePage, 2)
    T_NUMBER(ContactSetHomePage, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetHomePage, 1) // char*

    RETURN_NUMBER(ContactSetHomePage((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetPhoneNumber, 3)
    T_NUMBER(ContactSetPhoneNumber, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetPhoneNumber, 1) // char*
    T_NUMBER(ContactSetPhoneNumber, 2)      // int

    RETURN_NUMBER(ContactSetPhoneNumber((CMAPIContact *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetAddress, 3)
    T_NUMBER(ContactSetAddress, 0)     // CMAPIContact
    T_NUMBER(ContactSetAddress, 1)     // CContactAddress
    T_NUMBER(ContactSetAddress, 2)     // CContactAddress::AddressType

    RETURN_NUMBER(ContactSetAddress((CMAPIContact *)PARAM_INT(0), (CContactAddress *)PARAM_INT(1), (CContactAddress::AddressType)PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetPostalAddress, 2)
    T_NUMBER(ContactSetPostalAddress, 0)     // CMAPIContact
    T_NUMBER(ContactSetPostalAddress, 1)     // CContactAddress::AddressType

    RETURN_NUMBER(ContactSetPostalAddress((CMAPIContact *)PARAM_INT(0), (CContactAddress::AddressType)PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactUpdateDisplayAddress, 2)
    T_NUMBER(ContactUpdateDisplayAddress, 0)     // CMAPIContact
    T_NUMBER(ContactUpdateDisplayAddress, 1)     // CContactAddress::AddressType

    RETURN_NUMBER(ContactUpdateDisplayAddress((CMAPIContact *)PARAM_INT(0), (CContactAddress::AddressType)PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetNotes, 3)
    T_NUMBER(ContactSetNotes, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetNotes, 1) // char*
    T_NUMBER(ContactSetNotes, 2)      // int

    RETURN_NUMBER(ContactSetNotes((CMAPIContact *)PARAM_INT(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetSensitivity, 2)
    T_NUMBER(ContactSetSensitivity, 0)     // CMAPIContact
    T_NUMBER(ContactSetSensitivity, 1)     // int

    RETURN_NUMBER(ContactSetSensitivity((CMAPIContact *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetFileAs, 2)
    T_NUMBER(ContactSetFileAs, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetFileAs, 1) // char*

    RETURN_NUMBER(ContactSetFileAs((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetTitle, 2)
    T_NUMBER(ContactSetTitle, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetTitle, 1) // char*

    RETURN_NUMBER(ContactSetTitle((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetCompany, 2)
    T_NUMBER(ContactSetCompany, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetCompany, 1) // char*

    RETURN_NUMBER(ContactSetCompany((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetDisplayNamePrefix, 2)
    T_NUMBER(ContactSetDisplayNamePrefix, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetDisplayNamePrefix, 1) // char*

    RETURN_NUMBER(ContactSetDisplayNamePrefix((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetGeneration, 2)
    T_NUMBER(ContactSetGeneration, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetGeneration, 1) // char*

    RETURN_NUMBER(ContactSetGeneration((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactUpdateDisplayName, 1)
    T_NUMBER(ContactUpdateDisplayName, 0)     // CMAPIContact

    RETURN_NUMBER(ContactUpdateDisplayName((CMAPIContact *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetDepartment, 2)
    T_NUMBER(ContactSetDepartment, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetDepartment, 1) // char*

    RETURN_NUMBER(ContactSetDepartment((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetOffice, 2)
    T_NUMBER(ContactSetOffice, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetOffice, 1) // char*

    RETURN_NUMBER(ContactSetOffice((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetManagerName, 2)
    T_NUMBER(ContactSetManagerName, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetManagerName, 1) // char*

    RETURN_NUMBER(ContactSetManagerName((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetAssistantName, 2)
    T_NUMBER(ContactSetAssistantName, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetAssistantName, 1) // char*

    RETURN_NUMBER(ContactSetAssistantName((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetNickName, 2)
    T_NUMBER(ContactSetNickName, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetNickName, 1) // char*

    RETURN_NUMBER(ContactSetNickName((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetSpouseName, 2)
    T_NUMBER(ContactSetSpouseName, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetSpouseName, 1) // char*

    RETURN_NUMBER(ContactSetSpouseName((CMAPIContact *)PARAM_INT(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetBirthday, 4)
    T_NUMBER(ContactSetBirthday, 0)     // CMAPIContact
    T_NUMBER(ContactSetBirthday, 1)     // int
    T_NUMBER(ContactSetBirthday, 2)     // int
    T_NUMBER(ContactSetBirthday, 3)     // int

    RETURN_NUMBER(ContactSetBirthday((CMAPIContact *)PARAM_INT(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetAnniversary, 4)
    T_NUMBER(ContactSetAnniversary, 0)     // CMAPIContact
    T_NUMBER(ContactSetAnniversary, 1)     // int
    T_NUMBER(ContactSetAnniversary, 2)     // int
    T_NUMBER(ContactSetAnniversary, 3)     // int

    RETURN_NUMBER(ContactSetAnniversary((CMAPIContact *)PARAM_INT(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ContactSetNamedProperty, 4)
    T_NUMBER(ContactSetNamedProperty, 0)      // CMAPIContact
    T_STRING_NULL(ContactSetNamedProperty, 1) // char*
    T_STRING_NULL(ContactSetNamedProperty, 2) // char*
    T_NUMBER(ContactSetNamedProperty, 3)      // int

    RETURN_NUMBER(ContactSetNamedProperty((CMAPIContact *)PARAM_INT(0), PARAM(1), PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressClose, 1)
    T_NUMBER(AddressClose, 0)     // CContactAddress

    AddressClose((CContactAddress *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressGetStreet, 3)
    T_NUMBER(AddressGetStreet, 0)     // CContactAddress
//T_STRING_NULL(1) // char*
    T_NUMBER(AddressGetStreet, 2)     // int

    PROP(2)
    AddressGetStreet((CContactAddress *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressGetCity, 3)
    T_NUMBER(AddressGetCity, 0)     // CContactAddress
//T_STRING_NULL(1) // char*
    T_NUMBER(AddressGetCity, 2)     // int

    PROP(2)
    AddressGetCity((CContactAddress *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressGetStateOrProvince, 3)
    T_NUMBER(AddressGetStateOrProvince, 0)     // CContactAddress
//T_STRING_NULL(1) // char*
    T_NUMBER(AddressGetStateOrProvince, 2)     // int

    PROP(2)
    AddressGetStateOrProvince((CContactAddress *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressGetPostalCode, 3)
    T_NUMBER(AddressGetPostalCode, 0)     // CContactAddress
//T_STRING_NULL(1) // char*
    T_NUMBER(AddressGetPostalCode, 2)     // int

    PROP(2)
    AddressGetPostalCode((CContactAddress *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressGetCountry, 3)
    T_NUMBER(AddressGetCountry, 0)     // CContactAddress
//T_STRING_NULL(1) // char*
    T_NUMBER(AddressGetCountry, 2)     // int

    PROP(2)
    AddressGetCountry((CContactAddress *)PARAM_INT(0), value, (int)PARAM(2));
    RET(1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressSetStreet, 2)
    T_NUMBER(AddressSetStreet, 0)      // CContactAddress
    T_STRING_NULL(AddressSetStreet, 1) // char*

    AddressSetStreet((CContactAddress *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressSetCity, 2)
    T_NUMBER(AddressSetCity, 0)      // CContactAddress
    T_STRING_NULL(AddressSetCity, 1) // char*

    AddressSetCity((CContactAddress *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressSetStateOrProvince, 2)
    T_NUMBER(AddressSetStateOrProvince, 0)      // CContactAddress
    T_STRING_NULL(AddressSetStateOrProvince, 1) // char*

    AddressSetStateOrProvince((CContactAddress *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressSetPostalCode, 2)
    T_NUMBER(AddressSetPostalCode, 0)      // CContactAddress
    T_STRING_NULL(AddressSetPostalCode, 1) // char*

    AddressSetPostalCode((CContactAddress *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddressSetCountry, 2)
    T_NUMBER(AddressSetCountry, 0)      // CContactAddress
    T_STRING_NULL(AddressSetCountry, 1) // char*

    AddressSetCountry((CContactAddress *)PARAM_INT(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------


