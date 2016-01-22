//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

struct virtContainerData {
    char *username;
    char *password;
};

static void customGlobalErrorFunc(void *userdata, virErrorPtr err) {
    // nothing
}

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_DOMAINS_ACTIVE)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_DOMAINS_INACTIVE)
    DEFINE_ECONSTANT(VIR_MEMORY_VIRTUAL)
    DEFINE_ECONSTANT(VIR_MEMORY_PHYSICAL)

    DEFINE_ECONSTANT(VIR_DOMAIN_SHUTDOWN_DEFAULT)
    DEFINE_ECONSTANT(VIR_DOMAIN_SHUTDOWN_ACPI_POWER_BTN)
    DEFINE_ECONSTANT(VIR_DOMAIN_SHUTDOWN_GUEST_AGENT)
    DEFINE_ECONSTANT(VIR_DOMAIN_SHUTDOWN_INITCTL)
    DEFINE_ECONSTANT(VIR_DOMAIN_SHUTDOWN_SIGNAL)

    DEFINE_ECONSTANT(VIR_DOMAIN_UNDEFINE_MANAGED_SAVE)
    DEFINE_ECONSTANT(VIR_DOMAIN_UNDEFINE_SNAPSHOTS_METADATA)

    DEFINE_ECONSTANT(VIR_DOMAIN_REBOOT_DEFAULT)
    DEFINE_ECONSTANT(VIR_DOMAIN_REBOOT_ACPI_POWER_BTN)
    DEFINE_ECONSTANT(VIR_DOMAIN_REBOOT_GUEST_AGENT)
    DEFINE_ECONSTANT(VIR_DOMAIN_REBOOT_INITCTL)
    DEFINE_ECONSTANT(VIR_DOMAIN_REBOOT_SIGNAL)

    DEFINE_ECONSTANT(VIR_DOMAIN_NONE)
    DEFINE_ECONSTANT(VIR_DOMAIN_START_PAUSED)
    DEFINE_ECONSTANT(VIR_DOMAIN_START_AUTODESTROY)
    DEFINE_ECONSTANT(VIR_DOMAIN_START_BYPASS_CACHE)
    DEFINE_ECONSTANT(VIR_DOMAIN_START_FORCE_BOOT)

    DEFINE_ECONSTANT(VIR_KEYCODE_SET_LINUX)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_XT)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_ATSET1)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_ATSET2)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_ATSET3)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_OSX)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_XT_KBD)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_USB)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_WIN32)
    DEFINE_ECONSTANT(VIR_KEYCODE_SET_RFB)

    DEFINE_ECONSTANT(VIR_DOMAIN_CONSOLE_FORCE)
    DEFINE_ECONSTANT(VIR_DOMAIN_CONSOLE_SAFE)

    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_REDEFINE)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_CURRENT)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_NO_METADATA)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_HALT)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_DISK_ONLY)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_REUSE_EXT)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_QUIESCE)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_ATOMIC)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_CREATE_LIVE)

    DEFINE_ECONSTANT(VIR_DOMAIN_SAVE_BYPASS_CACHE)
    DEFINE_ECONSTANT(VIR_DOMAIN_SAVE_RUNNING)
    DEFINE_ECONSTANT(VIR_DOMAIN_SAVE_PAUSED)

    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_DESCENDANTS)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_ROOTS)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_METADATA)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_LEAVES)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_NO_LEAVES)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_NO_METADATA)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_INACTIVE)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_ACTIVE)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_DISK_ONLY)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_INTERNAL)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_LIST_EXTERNAL)

    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_REVERT_RUNNING)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_REVERT_PAUSED)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_REVERT_FORCE)

    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_DELETE_METADATA_ONLY)
    DEFINE_ECONSTANT(VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN_ONLY)

    DEFINE_ECONSTANT(VIR_DOMAIN_SAVE_BYPASS_CACHE)
    DEFINE_ECONSTANT(VIR_DOMAIN_SAVE_RUNNING)
    DEFINE_ECONSTANT(VIR_DOMAIN_SAVE_PAUSED)

    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_INACTIVE)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_ACTIVE)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_PERSISTENT)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_TRANSIENT)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_AUTOSTART)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_NO_AUTOSTART)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_DIR)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_FS)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_NETFS)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_LOGICAL)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_DISK)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_ISCSI)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_SCSI)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_MPATH)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_RBD)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_SHEEPDOG)
    DEFINE_ECONSTANT(VIR_CONNECT_LIST_STORAGE_POOLS_GLUSTER)

    DEFINE_ECONSTANT(VIR_DOMAIN_JOB_NONE)
    DEFINE_ECONSTANT(VIR_DOMAIN_JOB_BOUNDED)
    DEFINE_ECONSTANT(VIR_DOMAIN_JOB_UNBOUNDED)
    DEFINE_ECONSTANT(VIR_DOMAIN_JOB_COMPLETED)
    DEFINE_ECONSTANT(VIR_DOMAIN_JOB_FAILED)
    DEFINE_ECONSTANT(VIR_DOMAIN_JOB_CANCELLED)

    DEFINE_ECONSTANT(VIR_DOMAIN_AFFECT_CURRENT)
    DEFINE_ECONSTANT(VIR_DOMAIN_AFFECT_LIVE)
    DEFINE_ECONSTANT(VIR_DOMAIN_AFFECT_CONFIG)

    DEFINE_ECONSTANT(VIR_DOMAIN_DEVICE_MODIFY_CONFIG)
    DEFINE_ECONSTANT(VIR_DOMAIN_DEVICE_MODIFY_CURRENT)
    DEFINE_ECONSTANT(VIR_DOMAIN_DEVICE_MODIFY_LIVE)
    DEFINE_ECONSTANT(VIR_DOMAIN_DEVICE_MODIFY_FORCE)

    DEFINE_ECONSTANT(VIR_DOMAIN_BLOCK_JOB_ABORT_ASYNC)
    DEFINE_ECONSTANT(VIR_DOMAIN_BLOCK_JOB_ABORT_PIVOT)

    DEFINE_ECONSTANT(VIR_MIGRATE_LIVE)
    DEFINE_ECONSTANT(VIR_MIGRATE_PEER2PEER)
    DEFINE_ECONSTANT(VIR_MIGRATE_TUNNELLED)
    DEFINE_ECONSTANT(VIR_MIGRATE_PERSIST_DEST)
    DEFINE_ECONSTANT(VIR_MIGRATE_UNDEFINE_SOURCE)
    DEFINE_ECONSTANT(VIR_MIGRATE_PAUSED)
    DEFINE_ECONSTANT(VIR_MIGRATE_NON_SHARED_DISK)
    DEFINE_ECONSTANT(VIR_MIGRATE_NON_SHARED_INC)
    DEFINE_ECONSTANT(VIR_MIGRATE_CHANGE_PROTECTION)
    DEFINE_ECONSTANT(VIR_MIGRATE_UNSAFE)
    DEFINE_ECONSTANT(VIR_MIGRATE_OFFLINE)
    DEFINE_ECONSTANT(VIR_MIGRATE_COMPRESSED)
    DEFINE_ECONSTANT(VIR_MIGRATE_ABORT_ON_ERROR)
    DEFINE_ECONSTANT(VIR_MIGRATE_AUTO_CONVERGE)
    virSetErrorFunc(NULL, customGlobalErrorFunc);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
static int authCb(virConnectCredentialPtr cred, unsigned int ncred, void *cbdata) {
    int  i;
    char buf[1024];
    virtContainerData *LoginData = (virtContainerData *)cbdata;

    for (i = 0; i < ncred; i++) {
        if (cred[i].type == VIR_CRED_AUTHNAME) {
            cred[i].result = strdup(LoginData->username);
            if (cred[i].result == NULL)
                return -1;
            cred[i].resultlen = strlen(cred[i].result);
        } else if (cred[i].type == VIR_CRED_PASSPHRASE) {
            cred[i].result = strdup(LoginData->password);
            if (cred[i].result == NULL)
                return -1;
            cred[i].resultlen = strlen(cred[i].result);
        }
    }
    return 0;
}

//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virConnect, 1, 4)
    T_STRING(0)
    int read_only = 0;
    char       *username   = NULL;
    char       *password   = NULL;
    static int authCreds[] = {
        VIR_CRED_AUTHNAME,
        VIR_CRED_PASSPHRASE,
    };
    struct virtContainerData LoginData;

    if (PARAMETERS_COUNT > 1) {
        T_STRING(1);
        username = PARAM(1);
        if (PARAMETERS_COUNT > 2) {
            T_STRING(2);
            password = PARAM(2);
            if (PARAMETERS_COUNT > 3) {
                T_NUMBER(3);
                read_only = PARAM_INT(3);
            }
        }
    }
    LoginData.username = username;
    LoginData.password = password;

    virConnectPtr conn  = 0;
    char          *host = PARAM(0);
    if (!(PARAM_LEN(0)))
        host = NULL;

    virConnectAuth auth;

    auth.credtype  = authCreds;
    auth.ncredtype = sizeof(authCreds) / sizeof(int);
    auth.cb        = authCb;
    auth.cbdata    = &LoginData;

    conn = virConnectOpenAuth(host, &auth, read_only);
    RETURN_NUMBER((SYS_INT)conn);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virClose, 1)
    T_HANDLE(0)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    virConnectClose(conn);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virInfo, 1)
    T_HANDLE(0)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    unsigned long hvVer   = 0, major = 0, minor = 0, release = 0;
    const char    *hvType = virConnectGetType(conn);
    AnsiString    version;

    if (!virConnectGetVersion(conn, &hvVer)) {
        major    = hvVer / 1000000;
        hvVer   %= 1000000;
        minor    = hvVer / 1000;
        release  = hvVer % 1000;
        version += AnsiString((long)major);
        version += ".";
        version += AnsiString((long)minor);
        version += ".";
        version += AnsiString((long)release);
    }
    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_STRING, (char *)hvType, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "version", (INTEGER)VARIABLE_STRING, (char *)version.c_str(), (NUMBER)version.Length());

    virNodeInfo nodeinfo;
    if (!virNodeGetInfo(conn, &nodeinfo)) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "model", (INTEGER)VARIABLE_STRING, (char *)nodeinfo.model, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "memory", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.memory);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "cpus", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.cpus);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "mhz", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.mhz);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nodes", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.nodes);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sockets", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.sockets);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "cores/socket", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.cores);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "threads/core", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nodeinfo.threads);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virNumOfActiveDomains, 1)
    T_HANDLE(0)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    RETURN_NUMBER(virConnectNumOfDomains(conn));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virNumOfInactiveDomains, 1)
    T_HANDLE(0)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    RETURN_NUMBER(virConnectNumOfDefinedDomains(conn));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virListAllDomains, 1, 2)
    T_HANDLE(0)
    int flags = VIR_CONNECT_LIST_DOMAINS_ACTIVE | VIR_CONNECT_LIST_DOMAINS_INACTIVE;
    char         buf[VIR_UUID_STRING_BUFLEN + 1];
    virDomainPtr *nameList = NULL;

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }
    INTEGER numNames = virConnectListAllDomains(conn, &nameList, flags);

    CREATE_ARRAY(RESULT);
    for (INTEGER i = 0; i < numNames; i++) {
        void *data = 0;
        Invoke(INVOKE_ARRAY_VARIABLE, RESULT, i, &data);
        if (data) {
            CREATE_ARRAY(data);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "name", (INTEGER)VARIABLE_STRING, (char *)virDomainGetName(nameList[i]), (NUMBER)0);
            buf[0] = 0;
            virDomainGetUUIDString(nameList[i], buf);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "uuid", (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0);
            int id = virDomainGetID(nameList[i]);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)id);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "cpus", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainGetCPUStats(nameList[i], NULL, 0, 0, 0, 0));
            char *os = virDomainGetOSType(nameList[i]);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "os", (INTEGER)VARIABLE_STRING, (char *)os, (NUMBER)0);
            if (os)
                free(os);
            char *host = virDomainGetHostname(nameList[i], 0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "hostname", (INTEGER)VARIABLE_STRING, (char *)host, (NUMBER)0);
            if (host)
                free(host);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "maxmemory", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainGetMaxMemory(nameList[i]));
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "maxvcpus", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainGetMaxVcpus(nameList[i]));
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "active", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainIsActive(nameList[i]));
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "persistent", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainIsPersistent(nameList[i]));
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, "updated", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainIsUpdated(nameList[i]));
        }
        virDomainFree(nameList[i]);
    }
    free(nameList);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainOpen, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    virDomainPtr domain = virDomainLookupByID(conn, PARAM_INT(1));
    RETURN_NUMBER((SYS_INT)domain);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainOpenName, 2)
    T_HANDLE(0)
    T_STRING(1)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    virDomainPtr domain = virDomainLookupByName(conn, PARAM(1));
    RETURN_NUMBER((SYS_INT)domain);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainOpenUUID, 2)
    T_HANDLE(0)
    T_STRING(1)

    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    virDomainPtr domain = virDomainLookupByUUIDString(conn, PARAM(1));
    RETURN_NUMBER((SYS_INT)domain);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainDone, 1)
    T_HANDLE(0)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    virDomainFree(domain);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainMemoryPeek, 5)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(4)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int flags = PARAM_INT(4);

    char *buffer;
    int  size = PARAM_INT(2);
    CORE_NEW(size + 1, buffer);
    buffer[size] = 0;

    int res = virDomainMemoryPeek(domain, (SYS_INT)PARAM(1), size, buffer, flags);
    if (res < 0) {
        CORE_DELETE(buffer);
        SET_STRING(3, "");
    } else {
        SetVariable(PARAMETER(3), -1, buffer, size - 1);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainShutdown, 1, 2)
    T_HANDLE(0)
    int flags = VIR_DOMAIN_SHUTDOWN_DEFAULT;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainShutdownFlags(domain, flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainSetAutostart, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainSetAutostart(domain, PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainDestroy, 1, 2)
    T_HANDLE(0)
    int flags = VIR_DOMAIN_DESTROY_DEFAULT;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainDestroyFlags(domain, flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainUndefine, 1, 2)
    T_HANDLE(0)
    int flags = VIR_DOMAIN_UNDEFINE_MANAGED_SAVE;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainUndefineFlags(domain, flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainSuspend, 1)
    T_HANDLE(0)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainSuspend(domain);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainResume, 1)
    T_HANDLE(0)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainResume(domain);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainCreate, 1)
    T_HANDLE(0)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainCreate(domain);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainCreateXML, 2, 3)
    T_HANDLE(0)
    T_STRING(1)
    int flags = VIR_DOMAIN_NONE;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }

    virConnectPtr conn   = (virConnectPtr)(SYS_INT)PARAM(0);
    virDomainPtr  domain = virDomainCreateXML(conn, PARAM(1), flags);
    RETURN_NUMBER((SYS_INT)domain);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainScreenshot, 1, 3)
    T_HANDLE(0)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);

    int screen = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        screen = PARAM_INT(1);
    }

    virStreamPtr stream = virStreamNew(virDomainGetConnect(domain), 0);

    char *type = virDomainScreenshot(domain, stream, screen, 0);

    if (PARAMETERS_COUNT > 2) {
        if (type) {
            SET_STRING(2, type);
        } else {
            SET_STRING(2, "");
        }
    }
    char       buf[0xFFF];
    AnsiString res;
    while (true) {
        int size = virStreamRecv(stream, buf, sizeof(buf) > 0);
        if (size <= 0)
            break;
        res.AddBuffer(buf, size);
    }
    virStreamFinish(stream);
    virStreamFree(stream);
    if (res.Length()) {
        RETURN_BUFFER(res.c_str(), res.Length());
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainSendKey, 2, 4)
    T_HANDLE(0)
    T_STRING(1)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);

    int          len = PARAM_LEN(1);
    unsigned int keycodes[VIR_DOMAIN_SEND_KEY_MAX_KEYS];
    if (len >= VIR_DOMAIN_SEND_KEY_MAX_KEYS)
        len = VIR_DOMAIN_SEND_KEY_MAX_KEYS;

    int timeout = 50;
    int keyset  = VIR_KEYCODE_SET_LINUX;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        timeout = PARAM_INT(2);

        if (PARAMETERS_COUNT > 3) {
            T_NUMBER(3)
            keyset = PARAM_INT(3);
        }
    }

    unsigned char *codes = (unsigned char *)PARAM(1);
    for (int i = 0; i < len; i++)
        keycodes[i] = (int)codes[i];

    int res = virDomainSendKey(domain, keyset, timeout, keycodes, len, 0);
    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainOpenConsole, 1, 3)
    T_HANDLE(0)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);

    char *dev_name = NULL;
    int  flags     = VIR_DOMAIN_CONSOLE_SAFE;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        if (PARAM_LEN(1) > 0)
            dev_name = PARAM(1);
        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(2)
            flags = PARAM_INT(2);
        }
    }

    virStreamPtr stream = virStreamNew(virDomainGetConnect(domain), VIR_STREAM_NONBLOCK);
    if (virDomainOpenConsole(domain, dev_name, stream, flags)) {
        virStreamFree(stream);
        stream = 0;
    }
    RETURN_NUMBER((SYS_INT)stream);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virStreamClose, 1)
    T_HANDLE(0)
    virStreamPtr stream = (virStreamPtr)(SYS_INT)PARAM(0);
    virStreamFinish(stream);
    int res = virStreamFree(stream);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virStreamWrite, 2)
    T_HANDLE(0)
    T_STRING(1)
    virStreamPtr stream = (virStreamPtr)(SYS_INT)PARAM(0);
    int res = virStreamSend(stream, PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virStreamRead, 3)
    T_HANDLE(0)
    T_NUMBER(2)

    virStreamPtr stream = (virStreamPtr)(SYS_INT)PARAM(0);
    char *buffer;
    int  size = PARAM_INT(2);
    CORE_NEW(size + 1, buffer);
    buffer[size] = 0;

    int res = virStreamRecv(stream, buffer, size);
    if (res <= 0) {
        CORE_DELETE(buffer);
        SET_STRING(1, "");
    } else {
        SetVariable(PARAMETER(1), -1, buffer, res);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virError, 0)
    virErrorPtr err = virSaveLastError();
    CREATE_ARRAY(RESULT);
    if (err) {
        if (err->code) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "code", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)err->code);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "domain", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)err->domain);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "level", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)err->level);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "message", (INTEGER)VARIABLE_STRING, (char *)err->message, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "str1", (INTEGER)VARIABLE_STRING, (char *)err->str1, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "str2", (INTEGER)VARIABLE_STRING, (char *)err->str2, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "str3", (INTEGER)VARIABLE_STRING, (char *)err->str3, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "int1", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)err->int1);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "int2", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)err->int2);
        }
        virFreeError(err);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDebug, 1)
    T_NUMBER(0)
    if (PARAM_INT(0))
        virSetErrorFunc(NULL, NULL);
    else
        virSetErrorFunc(NULL, customGlobalErrorFunc);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainGetCPUStats, 4)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(3)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);

    int flags   = VIR_TYPED_PARAM_STRING_OKAY;
    int nparams = PARAM_INT(1);
    int ncpus   = PARAM_INT(3);
    virTypedParameterPtr params = NULL;
    if (nparams > 0) {
        if (ncpus <= 0)
            ncpus = 0;
        params = (virTypedParameterPtr)malloc(ncpus * nparams * sizeof(virTypedParameter));
    }

    int res = virDomainGetCPUStats(domain, params, nparams, PARAM_INT(2), ncpus, flags);
    CREATE_ARRAY(RESULT);
    if (res > 0) {
        for (INTEGER j = 0; j < ncpus; j++) {
            void *data = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, RESULT, j, &data);
            if (data) {
                CREATE_ARRAY(data);
                for (INTEGER i = 0; i < res; i++) {
                    switch (params[i].type) {
                        case VIR_TYPED_PARAM_INT:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)params[i].value.i);
                            break;

                        case VIR_TYPED_PARAM_UINT:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)params[i].value.ui);
                            break;

                        case VIR_TYPED_PARAM_LLONG:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)params[i].value.l);
                            break;

                        case VIR_TYPED_PARAM_ULLONG:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)params[i].value.ul);
                            break;

                        case VIR_TYPED_PARAM_DOUBLE:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)params[i].value.d);
                            break;

                        case VIR_TYPED_PARAM_BOOLEAN:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)params[i].value.b);
                            break;

                        case VIR_TYPED_PARAM_STRING:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, data, params[i].field, (INTEGER)VARIABLE_STRING, (char *)params[i].value.s, (NUMBER)0);
                            if (params[i].value.s)
                                free(params[i].value.s);
                            break;
                    }
                }
            }
        }
    }
    if (params)
        free(params);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainInterfaceStats, 2)
    T_HANDLE(0)
    T_STRING(1)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    struct _virDomainInterfaceStats stats;
    CREATE_ARRAY(RESULT);
    if (!virDomainInterfaceStats(domain, PARAM(1), &stats, sizeof(stats))) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rx_bytes", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.rx_bytes);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rx_packets", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.rx_packets);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rx_errs", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.rx_errs);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rx_drop", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.rx_drop);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tx_bytes", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.tx_bytes);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tx_packets", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.tx_packets);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tx_errs", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.tx_errs);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tx_drop", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.tx_drop);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainMemoryStats, 1)
    T_HANDLE(0)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    struct _virDomainMemoryStat stats[VIR_DOMAIN_MEMORY_STAT_NR];
    CREATE_ARRAY(RESULT);

    int nr_stats = virDomainMemoryStats(domain, stats, VIR_DOMAIN_MEMORY_STAT_NR, 0);
    for (INTEGER i = 0; i < nr_stats; i++) {
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_SWAP_IN)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "swap_in", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_SWAP_OUT)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "swap_out", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "major_fault", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "minor_fault", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_UNUSED)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "unused", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_AVAILABLE)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "available", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_ACTUAL_BALLOON)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "actual", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
        if (stats[i].tag == VIR_DOMAIN_MEMORY_STAT_RSS)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rss", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats[i].val);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainBlockStats, 2)
    T_HANDLE(0)
    T_STRING(1)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    struct _virDomainBlockStats stats;
    CREATE_ARRAY(RESULT);
    if (!virDomainBlockStats(domain, PARAM(1), &stats, sizeof(stats))) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rd_req", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.rd_req);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "rd_bytes", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.rd_bytes);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "wr_req", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.wr_req);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "wr_bytes", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.wr_bytes);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "errs", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stats.errs);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainSnapshotCreate, 1, 3)
    T_HANDLE(0)
    int flags = 0;
    char *str = "<domainsnapshot/>";
    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        if (PARAM_LEN(1) > 0)
            str = PARAM(1);

        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(2)
            flags = PARAM_INT(2);
        }
    }
    virDomainPtr         domain = (virDomainPtr)(SYS_INT)PARAM(0);
    virDomainSnapshotPtr res    = virDomainSnapshotCreateXML(domain, str, flags);

    if (res) {
        virDomainSnapshotFree(res);
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomain, 1)
    T_HANDLE(0)
    char buf[VIR_UUID_STRING_BUFLEN + 1];
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "name", (INTEGER)VARIABLE_STRING, (char *)virDomainGetName(domain), (NUMBER)0);
    buf[0] = 0;
    virDomainGetUUIDString(domain, buf);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "uuid", (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0);
    int id = virDomainGetID(domain);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "id", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)id);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "cpus", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainGetCPUStats(domain, NULL, 0, 0, 0, 0));
    char *os = virDomainGetOSType(domain);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "os", (INTEGER)VARIABLE_STRING, (char *)os, (NUMBER)0);
    if (os)
        free(os);
    char *host = virDomainGetHostname(domain, 0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "hostname", (INTEGER)VARIABLE_STRING, (char *)host, (NUMBER)0);
    if (host)
        free(host);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "maxmemory", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainGetMaxMemory(domain));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "maxvcpus", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainGetMaxVcpus(domain));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "active", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainIsActive(domain));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "persistent", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainIsPersistent(domain));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "updated", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainIsUpdated(domain));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainHasCurrentSnapshot, 1)
    T_HANDLE(0)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainHasCurrentSnapshot(domain, 0);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainSnapshotList, 1, 2)
    T_HANDLE(0)
    int flags = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          len    = virDomainSnapshotNum(domain, flags);
    CREATE_ARRAY(RESULT);
    if (len) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virDomainSnapshotListNames(domain, names, len, flags);
        for (INTEGER i = 0; i < res; i++) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
            free(names[i]);
        }
        free(names);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainSnapshot, 2)
    T_HANDLE(0)
    T_STRING(1)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    virDomainSnapshotPtr snapshot = virDomainSnapshotLookupByName(domain, PARAM(1), 0);
    CREATE_ARRAY(RESULT);
    if (snapshot) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "name", (INTEGER)VARIABLE_STRING, (char *)virDomainSnapshotGetName(snapshot), (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "is_current", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainSnapshotIsCurrent(snapshot, 0));
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "has_metadata", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)virDomainSnapshotHasMetadata(snapshot, 0));
        virDomainSnapshotPtr parent = virDomainSnapshotGetParent(snapshot, 0);
        if (parent) {
            const char *name = virDomainSnapshotGetName(parent);
            if (name)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "parent", (INTEGER)VARIABLE_STRING, (char *)name, (NUMBER)0);
            virDomainSnapshotFree(parent);
        }
        char *xml = virDomainSnapshotGetXMLDesc(snapshot, 0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "xml", (INTEGER)VARIABLE_STRING, (char *)xml, (NUMBER)0);
        if (xml)
            free(xml);

        virDomainSnapshotFree(snapshot);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainRevertToSnapshot, 2, 3)
    T_HANDLE(0)
    T_STRING(1)

    int flags = VIR_DOMAIN_SNAPSHOT_REVERT_RUNNING;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }

    virDomainPtr         domain   = (virDomainPtr)(SYS_INT)PARAM(0);
    virDomainSnapshotPtr snapshot = virDomainSnapshotLookupByName(domain, PARAM(1), 0);
    if (snapshot) {
        int res = virDomainRevertToSnapshot(snapshot, flags);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainSnapshotDelete, 2, 3)
    T_HANDLE(0)
    T_STRING(1)

    int flags = VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }

    virDomainPtr         domain   = (virDomainPtr)(SYS_INT)PARAM(0);
    virDomainSnapshotPtr snapshot = virDomainSnapshotLookupByName(domain, PARAM(1), 0);
    if (snapshot) {
        int res = virDomainSnapshotDelete(snapshot, flags);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainSnapshotListChildren, 1, 2)
    T_HANDLE(0)
    int flags = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }
    virDomainSnapshotPtr domain = (virDomainSnapshotPtr)(SYS_INT)PARAM(0);
    int len = virDomainSnapshotNumChildren(domain, flags);
    CREATE_ARRAY(RESULT);
    if (len) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virDomainSnapshotListChildrenNames(domain, names, len, flags);
        for (INTEGER i = 0; i < res; i++) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
            free(names[i]);
        }
        free(names);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainManagedSave, 1, 2)
    T_HANDLE(0)
    int flags = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainManagedSave(domain, flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainManagedSaveRemove, 1, 2)
    T_HANDLE(0)
    int flags = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        flags = PARAM_INT(1);
    }
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainManagedSaveRemove(domain, flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainSave, 2)
    T_HANDLE(0)
    T_STRING(1)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainSave(domain, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainHasManagedSaveImage, 1)
    T_HANDLE(0)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int res = virDomainHasManagedSaveImage(domain, 0);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virConnectListStoragePools, 1)
    T_HANDLE(0)
    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    int len = virConnectNumOfStoragePools(conn);

    CREATE_ARRAY(RESULT);
    if (len > 0) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virConnectListStoragePools(conn, names, len);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
        free(names);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virConnectListDefinedStoragePools, 1)
    T_HANDLE(0)
    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    int len = virConnectNumOfDefinedStoragePools(conn);

    CREATE_ARRAY(RESULT);
    if (len > 0) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virConnectListDefinedStoragePools(conn, names, len);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
        free(names);
    }
END_IMPL
//-----------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virConnectListNetworks, 1)
    T_HANDLE(0)
    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    int len = virConnectNumOfNetworks(conn);

    CREATE_ARRAY(RESULT);
    if (len > 0) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virConnectListNetworks(conn, names, len);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
        free(names);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virConnectListDefinedNetworks, 1)
    T_HANDLE(0)
    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    int len = virConnectNumOfDefinedNetworks(conn);

    CREATE_ARRAY(RESULT);
    if (len > 0) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virConnectListDefinedNetworks(conn, names, len);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
        free(names);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(virDomainGetJobInfo, 1)
    T_HANDLE(0)
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    virDomainJobInfo info;
    int res = virDomainGetJobInfo(domain, &info);
    if (!res) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.type);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "timeElapsed", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.timeElapsed);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "timeRemaining", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.timeRemaining);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "dataTotal", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.dataTotal);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "dataProcessed", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.dataProcessed);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "dataRemaining", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.dataRemaining);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "memTotal", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.memTotal);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "memProcessed", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.memProcessed);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "memRemaining", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.memRemaining);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "fileTotal", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.fileTotal);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "fileProcessed", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.fileProcessed);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "fileRemaining", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)info.fileRemaining);
    }
    CREATE_ARRAY(RESULT);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainAttachDevice, 2, 3)
    T_HANDLE(0)
    T_STRING(1)

    int flags = VIR_DOMAIN_AFFECT_CURRENT;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainAttachDeviceFlags(domain, PARAM(1), flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainBlockJobAbort, 2, 3)
    T_HANDLE(0)
    T_STRING(1)

    int flags = VIR_DOMAIN_BLOCK_JOB_ABORT_ASYNC;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainBlockJobAbort(domain, PARAM(1), flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainDetachDeviceFlags, 2, 3)
    T_HANDLE(0)
    T_STRING(1)

    int flags = VIR_DOMAIN_DEVICE_MODIFY_CURRENT;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }
    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    int          res    = virDomainAttachDeviceFlags(domain, PARAM(1), flags);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virNodeListDevices, 1, 2)
    T_HANDLE(0)
    char *cap = NULL;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        if (PARAM_LEN(1))
            cap = PARAM(1);
    }
    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(0);
    int           len  = virNodeNumOfDevices(conn, cap, 0);

    CREATE_ARRAY(RESULT);
    if (len > 0) {
        char **names = (char **)malloc(sizeof(char *) * len);
        int  res     = virNodeListDevices(conn, cap, names, len, 0);
        for (INTEGER i = 0; i < res; i++) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, (char *)names[i], (NUMBER)0);
        }
        free(names);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(virDomainMigrate, 2, 6)
    T_HANDLE(0)
    T_HANDLE(1)

    virDomainPtr domain = (virDomainPtr)(SYS_INT)PARAM(0);
    virConnectPtr conn = (virConnectPtr)(SYS_INT)PARAM(1);

    unsigned long flags     = VIR_MIGRATE_LIVE;
    char          *dname    = 0;
    char          *uri      = 0;
    unsigned long bandwidth = 0;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
        if (PARAMETERS_COUNT > 3) {
            T_STRING(3)
            if (PARAM_LEN(3) > 0)
                dname = PARAM(3);

            if (PARAMETERS_COUNT > 4) {
                T_STRING(4)
                if (PARAM_LEN(4) > 0)
                    uri = PARAM(4);

                if (PARAMETERS_COUNT > 5) {
                    T_NUMBER(5)
                    bandwidth = PARAM_INT(5);
                }
            }
        }
    }
    virDomainPtr domain2 = virDomainMigrate(domain, conn, flags, dname, uri, bandwidth);
    if (domain2) {
        virDomainFree(domain2);
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//------------------------------------------------------------------------
