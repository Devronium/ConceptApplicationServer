//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WINSOCKAPI_
 #include <windows.h>
 #include <io.h>
 #include <winsock2.h>
#else
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/un.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <string.h>
 #include <arpa/inet.h>
 #include <signal.h>
#endif
#include <modbus.h>
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
 #ifndef SO_NOSIGPIPE
    signal(SIGPIPE, SIG_IGN);
 #endif
#endif
    DEFINE_ECONSTANT(MODBUS_ERROR_RECOVERY_NONE);
    DEFINE_ECONSTANT(MODBUS_ERROR_RECOVERY_LINK);
    DEFINE_ECONSTANT(MODBUS_ERROR_RECOVERY_PROTOCOL);

    DEFINE_ECONSTANT(MODBUS_BROADCAST_ADDRESS)
    DEFINE_ECONSTANT(MODBUS_MAX_READ_BITS)
    DEFINE_ECONSTANT(MODBUS_MAX_WRITE_BITS)
    DEFINE_ECONSTANT(MODBUS_MAX_READ_REGISTERS)
    DEFINE_ECONSTANT(MODBUS_MAX_WRITE_REGISTERS)
    DEFINE_ECONSTANT(MODBUS_MAX_RW_WRITE_REGISTERS)
    DEFINE_ECONSTANT(MODBUS_ENOBASE)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_ILLEGAL_FUNCTION)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_ACKNOWLEDGE)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_MEMORY_PARITY)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_NOT_DEFINED)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_GATEWAY_PATH)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_GATEWAY_TARGET)
    DEFINE_ECONSTANT(MODBUS_EXCEPTION_MAX)
    DEFINE_ECONSTANT(EMBXILFUN)
    DEFINE_ECONSTANT(EMBXILADD)
    DEFINE_ECONSTANT(EMBXILVAL)
    DEFINE_ECONSTANT(EMBXSFAIL)
    DEFINE_ECONSTANT(EMBXACK)
    DEFINE_ECONSTANT(EMBXSBUSY)
    DEFINE_ECONSTANT(EMBXNACK)
    DEFINE_ECONSTANT(EMBXMEMPAR)
    DEFINE_ECONSTANT(EMBXGPATH)
    DEFINE_ECONSTANT(EMBXGTAR)
    DEFINE_ECONSTANT(EMBBADCRC)
    DEFINE_ECONSTANT(EMBBADDATA)
    DEFINE_ECONSTANT(EMBBADEXC)
    DEFINE_ECONSTANT(EMBUNKEXC)
    DEFINE_ECONSTANT(EMBMDATA)
    DEFINE_ECONSTANT(MODBUS_RTU_RS232)
    DEFINE_ECONSTANT(MODBUS_RTU_RS485)
    DEFINE_ECONSTANT(MODBUS_TCP_DEFAULT_PORT)
    DEFINE_ECONSTANT(MODBUS_TCP_SLAVE)
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(modbus_get_response_timeout, 1)
    T_HANDLE(modbus_get_response_timeout, 0)     // modbus_t*

    timeval local_parameter_1;
    modbus_get_response_timeout((modbus_t *)(SYS_INT)PARAM(0), &local_parameter_1);

    SYS_INT res = local_parameter_1.tv_sec * 1000000 + local_parameter_1.tv_usec;
    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_response_timeout, 2)
    T_HANDLE(modbus_set_response_timeout, 0)     // modbus_t*
    T_NUMBER(modbus_set_response_timeout, 1)

    SYS_INT useconds = PARAM_INT(1);
    timeval local_parameter_1;
    local_parameter_1.tv_sec  = useconds / 1000000;
    local_parameter_1.tv_usec = useconds % 1000000;

    modbus_set_response_timeout((modbus_t *)(SYS_INT)PARAM(0), &local_parameter_1);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_get_byte_timeout, 1)
    T_HANDLE(modbus_get_byte_timeout, 0)     // modbus_t*

    timeval local_parameter_1;
    modbus_get_byte_timeout((modbus_t *)(SYS_INT)PARAM(0), &local_parameter_1);

    SYS_INT res = local_parameter_1.tv_sec * 1000000 + local_parameter_1.tv_usec;
    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_byte_timeout, 2)
    T_HANDLE(modbus_set_byte_timeout, 0)     // modbus_t*
    T_NUMBER(modbus_set_byte_timeout, 1)

    SYS_INT useconds = PARAM_INT(1);
    timeval local_parameter_1;
    local_parameter_1.tv_sec  = useconds / 1000000;
    local_parameter_1.tv_usec = useconds % 1000000;

    modbus_set_byte_timeout((modbus_t *)(SYS_INT)PARAM(0), &local_parameter_1);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_slave, 2)
    T_HANDLE(modbus_set_slave, 0)     // modbus_t*
    T_NUMBER(modbus_set_slave, 1)     // int

    RETURN_NUMBER(modbus_set_slave((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_error_recovery, 2)
    T_HANDLE(modbus_set_error_recovery, 0)     // modbus_t*
    T_NUMBER(modbus_set_error_recovery, 1)     // modbus_error_recovery_mode

    RETURN_NUMBER(modbus_set_error_recovery((modbus_t *)(SYS_INT)PARAM(0), (modbus_error_recovery_mode)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_socket, 2)
    T_HANDLE(modbus_set_socket, 0)     // modbus_t*
    T_NUMBER(modbus_set_socket, 1)     // int

    modbus_set_socket((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_get_socket, 1)
    T_HANDLE(modbus_get_socket, 0)     // modbus_t*

    RETURN_NUMBER(modbus_get_socket((modbus_t *)(SYS_INT)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_get_header_length, 1)
    T_HANDLE(modbus_get_header_length, 0)     // modbus_t*

    RETURN_NUMBER(modbus_get_header_length((modbus_t *)(SYS_INT)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_connect, 1)
    T_HANDLE(modbus_connect, 0)     // modbus_t*

    RETURN_NUMBER(modbus_connect((modbus_t *)(SYS_INT)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_close, 1)
    T_HANDLE(modbus_close, 0)     // modbus_t*

    modbus_close((modbus_t *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_free, 1)
    T_HANDLE(modbus_free, 0)     // modbus_t*

    modbus_free((modbus_t *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_flush, 1)
    T_HANDLE(modbus_flush, 0)     // modbus_t*

    RETURN_NUMBER(modbus_flush((modbus_t *)(SYS_INT)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_debug, 2)
    T_HANDLE(modbus_set_debug, 0)     // modbus_t*
    T_NUMBER(modbus_set_debug, 1)     // int

    modbus_set_debug((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_strerror, 1)
    T_NUMBER(modbus_strerror, 0)     // int

    RETURN_STRING((char *)modbus_strerror((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_read_bits, 4)
    T_HANDLE(modbus_read_bits, 0)     // modbus_t*
    T_NUMBER(modbus_read_bits, 1)     // int
    T_NUMBER(modbus_read_bits, 2)     // int

    CREATE_ARRAY(PARAMETER(3));
    unsigned char *bits = (unsigned char *)malloc(PARAM_INT(2) * sizeof(unsigned char));
    if (bits) {
        int res = modbus_read_bits((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)PARAM(2), (unsigned char *)bits);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(3), i, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(bits[i] != 0));
        free(bits);
        RETURN_NUMBER(res)
    }  else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_read_input_bits, 4)
    T_HANDLE(modbus_read_input_bits, 0)     // modbus_t*
    T_NUMBER(modbus_read_input_bits, 1)     // int
    T_NUMBER(modbus_read_input_bits, 2)     // int

    CREATE_ARRAY(PARAMETER(3));
    unsigned char *bits = (unsigned char *)malloc(PARAM_INT(2) * sizeof(unsigned char));
    if (bits) {
        int res = modbus_read_input_bits((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)PARAM(2), (unsigned char *)bits);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(3), i, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(bits[i] != 0));
        free(bits);
        RETURN_NUMBER(res)
    }  else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_read_registers, 4)
    T_HANDLE(modbus_read_registers, 0)     // modbus_t*
    T_NUMBER(modbus_read_registers, 1)     // int
    T_NUMBER(modbus_read_registers, 2)     // int

    CREATE_ARRAY(PARAMETER(3));
    unsigned short *bits = (unsigned short *)malloc(PARAM_INT(2) * sizeof(unsigned short));
    if (bits) {
        int res = modbus_read_registers((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)PARAM(2), (unsigned short *)bits);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(3), i, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)bits[i]);
        free(bits);
        RETURN_NUMBER(res)
    }  else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_read_input_registers, 4)
    T_HANDLE(modbus_read_input_registers, 0)     // modbus_t*
    T_NUMBER(modbus_read_input_registers, 1)     // int
    T_NUMBER(modbus_read_input_registers, 2)     // int

    CREATE_ARRAY(PARAMETER(3));
    unsigned short *bits = (unsigned short *)malloc(PARAM_INT(2) * sizeof(unsigned short));
    if (bits) {
        int res = modbus_read_input_registers((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)PARAM(2), (unsigned short *)bits);
        for (INTEGER i = 0; i < res; i++)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(3), i, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)bits[i]);
        free(bits);
        RETURN_NUMBER(res)
    }  else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_write_bit, 3)
    T_HANDLE(modbus_write_bit, 0)     // modbus_t*
    T_NUMBER(modbus_write_bit, 1)     // int
    T_NUMBER(modbus_write_bit, 2)     // int

    RETURN_NUMBER(modbus_write_bit((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_write_register, 3)
    T_HANDLE(modbus_write_register, 0)     // modbus_t*
    T_NUMBER(modbus_write_register, 1)     // int
    T_NUMBER(modbus_write_register, 2)     // int

    RETURN_NUMBER(modbus_write_register((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_write_bits, 3)
    T_HANDLE(modbus_write_bits, 0)    // modbus_t*
    T_NUMBER(modbus_write_bits, 1)    // int
    T_ARRAY(modbus_write_bits, 2)     // unsigned char*

    int size = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    if (size > 0) {
        unsigned char *ret = (unsigned char *)malloc(size * sizeof(unsigned char));
        for (int i = 0; i < size; i++) {
            INTEGER type      = 0;
            NUMBER  nr        = 0;
            void    *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    ret[i] = 0;
                else
                    ret[i] = (unsigned char)(int)nData;
            }
        }
        int res = modbus_write_bits((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)size, (unsigned char *)ret);
        free(ret);
        RETURN_NUMBER(res)
    } else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_write_registers, 3)
    T_HANDLE(modbus_write_registers, 0)    // modbus_t*
    T_NUMBER(modbus_write_registers, 1)    // int
    T_ARRAY(modbus_write_registers, 2)     // unsigned short*

    int size = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    if (size > 0) {
        unsigned short *ret = (unsigned short *)malloc(size * sizeof(unsigned short));
        for (int i = 0; i < size; i++) {
            INTEGER type      = 0;
            NUMBER  nr        = 0;
            void    *newpData = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    ret[i] = 0;
                else
                    ret[i] = (unsigned short)nData;
            }
        }
        int res = modbus_write_registers((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)size, (unsigned short *)ret);
        free(ret);
        RETURN_NUMBER(res)
    } else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_write_and_read_registers, 6)
    T_HANDLE(modbus_write_and_read_registers, 0)     // modbus_t*
    T_NUMBER(modbus_write_and_read_registers, 1)     // int
    T_ARRAY(modbus_write_and_read_registers, 2)
    T_NUMBER(modbus_write_and_read_registers, 3)     // int
    T_NUMBER(modbus_write_and_read_registers, 4)     // int

    CREATE_ARRAY(PARAMETER(5));
    int size = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    if (size > 0) {
        unsigned short *ret = (unsigned short *)malloc(size * sizeof(unsigned short));
        if (ret) {
            for (int i = 0; i < size; i++) {
                INTEGER type      = 0;
                NUMBER  nr        = 0;
                void    *newpData = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &newpData);
                if (newpData) {
                    char    *szData;
                    INTEGER type;
                    NUMBER  nData;

                    Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if (type == VARIABLE_STRING)
                        ret[i] = 0;
                    else
                        ret[i] = (unsigned short)nData;
                }
            }
            unsigned short *bits = (unsigned short *)malloc(PARAM_INT(4) * sizeof(unsigned short));
            int            res   = modbus_write_and_read_registers((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1), (int)size, (unsigned short *)ret, PARAM_INT(3), PARAM_INT(4), bits);

            if (bits) {
                for (INTEGER i = 0; i < res; i++)
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(5), i, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)bits[i]);
                free(bits);
            }
            free(ret);
            RETURN_NUMBER(res)
        } else {
            RETURN_NUMBER(-3);
        }
    } else {
        RETURN_NUMBER(-2);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_report_slave_id, 2)
    T_HANDLE(modbus_report_slave_id, 0)     // modbus_t*

    unsigned char onoff = 0;
    RETURN_NUMBER(modbus_report_slave_id((modbus_t *)(SYS_INT)PARAM(0), &onoff))
    SET_NUMBER(1, onoff);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_mapping_new, 4)
    T_NUMBER(modbus_mapping_new, 0)     // int
    T_NUMBER(modbus_mapping_new, 1)     // int
    T_NUMBER(modbus_mapping_new, 2)     // int
    T_NUMBER(modbus_mapping_new, 3)     // int

    RETURN_NUMBER((SYS_INT)modbus_mapping_new((int)PARAM(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_mapping_free, 1)
    T_HANDLE(modbus_mapping_free, 0)     // modbus_mapping_t*

    modbus_mapping_free((modbus_mapping_t *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_send_raw_request, 2)
    T_HANDLE(modbus_send_raw_request, 0)     // modbus_t*
    T_STRING(modbus_send_raw_request, 1)     // unsigned char*

    RETURN_NUMBER(modbus_send_raw_request((modbus_t *)(SYS_INT)PARAM(0), (unsigned char *)PARAM(1), (int)PARAM_LEN(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_receive, 2)
    T_HANDLE(modbus_receive, 0)     // modbus_t*

    unsigned char query[0x200];
    int len = modbus_receive((modbus_t *)(SYS_INT)PARAM(0), query);
    if (len > 0) {
        SET_BUFFER(1, (char *)query, len);
    } else {
        SET_STRING(1, "");
    }
    RETURN_NUMBER(len)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(modbus_receive_from,3)
        T_HANDLE(modbus_receive, 0) // modbus_t*
    T_NUMBER(modbus_receive, 1)

    unsigned char query[0x200];
    int len=modbus_receive_from((modbus_t*)(SYS_INT)PARAM(0), (int)PARAM_INT(1), query);
    if (len>0) {
        SET_BUFFER(2, (char *)query, len);
    } else {
        SET_STRING(2, "");
    }
        RETURN_NUMBER(len)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_receive_confirmation, 2)
    T_HANDLE(modbus_receive_confirmation, 0)     // modbus_t*
    T_STRING(modbus_receive_confirmation, 1)     // unsigned char*

    RETURN_NUMBER(modbus_receive_confirmation((modbus_t *)(SYS_INT)PARAM(0), (unsigned char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_reply, 3)
    T_HANDLE(modbus_reply, 0)     // modbus_t*
    T_STRING(modbus_reply, 1)     // unsigned char*
    T_HANDLE(modbus_reply, 2)     // modbus_mapping_t*

    RETURN_NUMBER(modbus_reply((modbus_t *)(SYS_INT)PARAM(0), (unsigned char *)PARAM(1), (int)PARAM_LEN(1), (modbus_mapping_t *)(SYS_INT)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_reply_exception, 3)
    T_HANDLE(modbus_reply_exception, 0)     // modbus_t*
    T_STRING(modbus_reply_exception, 1)     // unsigned char*
    T_NUMBER(modbus_reply_exception, 2)     // unsigned int

    RETURN_NUMBER(modbus_reply_exception((modbus_t *)(SYS_INT)PARAM(0), (unsigned char *)PARAM(1), (unsigned int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_bits_from_byte, 3)
    T_STRING(modbus_set_bits_from_byte, 0)     // unsigned char*
    T_NUMBER(modbus_set_bits_from_byte, 1)     // int
    T_NUMBER(modbus_set_bits_from_byte, 2)     // unsigned char

    modbus_set_bits_from_byte((unsigned char *)PARAM(0), (int)PARAM(1), (unsigned char)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_bits_from_bytes, 4)
    T_STRING(modbus_set_bits_from_bytes, 0)     // unsigned char*
    T_NUMBER(modbus_set_bits_from_bytes, 1)     // int
    T_NUMBER(modbus_set_bits_from_bytes, 2)     // unsigned int
    T_STRING(modbus_set_bits_from_bytes, 3)     // unsigned char*

    modbus_set_bits_from_bytes((unsigned char *)PARAM(0), (int)PARAM(1), (unsigned int)PARAM(2), (unsigned char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_get_byte_from_bits, 3)
    T_STRING(modbus_get_byte_from_bits, 0)     // unsigned char*
    T_NUMBER(modbus_get_byte_from_bits, 1)     // int
    T_NUMBER(modbus_get_byte_from_bits, 2)     // unsigned int

    RETURN_NUMBER(modbus_get_byte_from_bits((unsigned char *)PARAM(0), (int)PARAM(1), (unsigned int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_get_float, 1)
    T_STRING(modbus_get_float, 0)
    if (PARAM_LEN(0) >= sizeof(unsigned short) * 2) {
        unsigned short *local_parameter_0 = (unsigned short *)PARAM(0);

        RETURN_NUMBER(modbus_get_float(local_parameter_0))
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_set_float, 2)
    T_NUMBER(modbus_set_float, 0)     // float

    unsigned short local_parameter_1[2];
    modbus_set_float((float)PARAM(0), local_parameter_1);

    SET_BUFFER(1, (char *)local_parameter_1, sizeof(unsigned short) * 2);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_new_rtu, 5)
    T_STRING(modbus_new_rtu, 0)     // char*
    T_NUMBER(modbus_new_rtu, 1)     // int
    T_NUMBER(modbus_new_rtu, 2)     // char
    T_NUMBER(modbus_new_rtu, 3)     // int
    T_NUMBER(modbus_new_rtu, 4)     // int

    RETURN_NUMBER((SYS_INT)modbus_new_rtu(PARAM(0), (int)PARAM(1), (char)PARAM(2), (int)PARAM(3), (int)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_rtu_set_serial_mode, 2)
    T_HANDLE(modbus_rtu_set_serial_mode, 0)     // modbus_t*
    T_NUMBER(modbus_rtu_set_serial_mode, 1)     // int

    RETURN_NUMBER(modbus_rtu_set_serial_mode((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_rtu_get_serial_mode, 1)
    T_HANDLE(modbus_rtu_get_serial_mode, 0)     // modbus_t*

    RETURN_NUMBER(modbus_rtu_get_serial_mode((modbus_t *)(SYS_INT)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_new_tcp, 2)
    T_STRING(modbus_new_tcp, 0)     // char*
    T_NUMBER(modbus_new_tcp, 1)     // int

    RETURN_NUMBER((SYS_INT)modbus_new_tcp(PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_tcp_listen, 2)
    T_HANDLE(modbus_tcp_listen, 0)     // modbus_t*
    T_NUMBER(modbus_tcp_listen, 1)     // int

    RETURN_NUMBER(modbus_tcp_listen((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_tcp_accept, 2)
    T_HANDLE(modbus_tcp_accept, 0)     // modbus_t*

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER(modbus_tcp_accept((modbus_t *)(SYS_INT)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (SYS_INT)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_new_tcp_pi, 2)
    T_STRING(modbus_new_tcp_pi, 0)     // char*
    T_STRING(modbus_new_tcp_pi, 1)     // char*

    RETURN_NUMBER((SYS_INT)modbus_new_tcp_pi(PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_tcp_pi_listen, 2)
    T_HANDLE(modbus_tcp_pi_listen, 0)     // modbus_t*
    T_NUMBER(modbus_tcp_pi_listen, 1)     // int

    RETURN_NUMBER(modbus_tcp_pi_listen((modbus_t *)(SYS_INT)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(modbus_tcp_pi_accept, 2)
    T_HANDLE(modbus_tcp_pi_accept, 0)     // modbus_t*

// ... parameter 1 is by reference (int*)
    int local_parameter_1;

    RETURN_NUMBER(modbus_tcp_pi_accept((modbus_t *)(SYS_INT)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (SYS_INT)local_parameter_1)
END_IMPL

