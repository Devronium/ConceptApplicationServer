//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

extern "C" {
#include <ibase.h>
#include <iberror.h>
}
#define DEFAULT_NCOLS              10

#define DATE_STRING_LENGTH         20
#define TIME_STRING_LENGTH         20
#define TIMESTAMP_STRING_LENGTH    36
#define NUMBER_LENGTH              40

typedef struct {
    isc_db_handle handle;
    ISC_STATUS    status_vector[20];
} DBHandle;

typedef struct {
    isc_stmt_handle stmt;
    XSQLDA          *row;
    isc_tr_handle   tran;
    DBHandle        *db_handle;
} QUERY;
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    // starting up windows sockets
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif
    //status_vector[0]=0;

    DEFINE_ECONSTANT(isc_facility)                         // 20;
    DEFINE_ECONSTANT(isc_base)                             // 335544320L;
    DEFINE_ECONSTANT(isc_factor)                           // 1;
    DEFINE_ECONSTANT(isc_arg_end)                          // 0;	// end of argument list
    DEFINE_ECONSTANT(isc_arg_gds)                          // 1;	// generic DSRI status value
    DEFINE_ECONSTANT(isc_arg_string)                       // 2;	// string argument
    DEFINE_ECONSTANT(isc_arg_cstring)                      // 3;	// count & string argument
    DEFINE_ECONSTANT(isc_arg_number)                       // 4;	// numeric argument (long)
    DEFINE_ECONSTANT(isc_arg_interpreted)                  // 5;	// interpreted status code (string)
    DEFINE_ECONSTANT(isc_arg_vms)                          // 6;	// VAX/VMS status code (long)
    DEFINE_ECONSTANT(isc_arg_unix)                         // 7;	// UNIX error code
    DEFINE_ECONSTANT(isc_arg_domain)                       // 8;	// Apollo/Domain error code
    DEFINE_ECONSTANT(isc_arg_dos)                          // 9;	// MSDOS/OS2 error code
    DEFINE_ECONSTANT(isc_arg_mpexl)                        // 10;	// HP MPE/XL error code
    DEFINE_ECONSTANT(isc_arg_mpexl_ipc)                    // 11;	// HP MPE/XL IPC error code
    DEFINE_ECONSTANT(isc_arg_next_mach)                    // 15;	// NeXT/Mach error code
    DEFINE_ECONSTANT(isc_arg_netware)                      // 16;	// NetWare error code
    DEFINE_ECONSTANT(isc_arg_win32)                        // 17;	// Win32 error code
    DEFINE_ECONSTANT(isc_arg_warning)                      // 18;	// warning argument

    DEFINE_ECONSTANT(isc_arith_except)                     // 335544321L;
    DEFINE_ECONSTANT(isc_bad_dbkey)                        // 335544322L;
    DEFINE_ECONSTANT(isc_bad_db_format)                    // 335544323L;
    DEFINE_ECONSTANT(isc_bad_db_handle)                    // 335544324L;
    DEFINE_ECONSTANT(isc_bad_dpb_content)                  // 335544325L;
    DEFINE_ECONSTANT(isc_bad_dpb_form)                     // 335544326L;
    DEFINE_ECONSTANT(isc_bad_req_handle)                   // 335544327L;
    DEFINE_ECONSTANT(isc_bad_segstr_handle)                // 335544328L;
    DEFINE_ECONSTANT(isc_bad_segstr_id)                    // 335544329L;
    DEFINE_ECONSTANT(isc_bad_tpb_content)                  // 335544330L;
    DEFINE_ECONSTANT(isc_bad_tpb_form)                     // 335544331L;
    DEFINE_ECONSTANT(isc_bad_trans_handle)                 // 335544332L;
    DEFINE_ECONSTANT(isc_bug_check)                        // 335544333L;
    DEFINE_ECONSTANT(isc_convert_error)                    // 335544334L;
    DEFINE_ECONSTANT(isc_db_corrupt)                       // 335544335L;
    DEFINE_ECONSTANT(isc_deadlock)                         // 335544336L;
    DEFINE_ECONSTANT(isc_excess_trans)                     // 335544337L;
    DEFINE_ECONSTANT(isc_from_no_match)                    // 335544338L;
    DEFINE_ECONSTANT(isc_infinap)                          // 335544339L;
    DEFINE_ECONSTANT(isc_infona)                           // 335544340L;
    DEFINE_ECONSTANT(isc_infunk)                           // 335544341L;
    DEFINE_ECONSTANT(isc_integ_fail)                       // 335544342L;
    DEFINE_ECONSTANT(isc_invalid_blr)                      // 335544343L;
    DEFINE_ECONSTANT(isc_io_error)                         // 335544344L;
    DEFINE_ECONSTANT(isc_lock_conflict)                    // 335544345L;
    DEFINE_ECONSTANT(isc_metadata_corrupt)                 // 335544346L;
    DEFINE_ECONSTANT(isc_not_valid)                        // 335544347L;
    DEFINE_ECONSTANT(isc_no_cur_rec)                       // 335544348L;
    DEFINE_ECONSTANT(isc_no_dup)                           // 335544349L;
    DEFINE_ECONSTANT(isc_no_finish)                        // 335544350L;
    DEFINE_ECONSTANT(isc_no_meta_update)                   // 335544351L;
    DEFINE_ECONSTANT(isc_no_priv)                          // 335544352L;
    DEFINE_ECONSTANT(isc_no_recon)                         // 335544353L;
    DEFINE_ECONSTANT(isc_no_record)                        // 335544354L;
    DEFINE_ECONSTANT(isc_no_segstr_close)                  // 335544355L;
    DEFINE_ECONSTANT(isc_obsolete_metadata)                // 335544356L;
    DEFINE_ECONSTANT(isc_open_trans)                       // 335544357L;
    DEFINE_ECONSTANT(isc_port_len)                         // 335544358L;
    DEFINE_ECONSTANT(isc_read_only_field)                  // 335544359L;
    DEFINE_ECONSTANT(isc_read_only_rel)                    // 335544360L;
    DEFINE_ECONSTANT(isc_read_only_trans)                  // 335544361L;
    DEFINE_ECONSTANT(isc_read_only_view)                   // 335544362L;
    DEFINE_ECONSTANT(isc_req_no_trans)                     // 335544363L;
    DEFINE_ECONSTANT(isc_req_sync)                         // 335544364L;
    DEFINE_ECONSTANT(isc_req_wrong_db)                     // 335544365L;
    DEFINE_ECONSTANT(isc_segment)                          // 335544366L;
    DEFINE_ECONSTANT(isc_segstr_eof)                       // 335544367L;
    DEFINE_ECONSTANT(isc_segstr_no_op)                     // 335544368L;
    DEFINE_ECONSTANT(isc_segstr_no_read)                   // 335544369L;
    DEFINE_ECONSTANT(isc_segstr_no_trans)                  // 335544370L;
    DEFINE_ECONSTANT(isc_segstr_no_write)                  // 335544371L;
    DEFINE_ECONSTANT(isc_segstr_wrong_db)                  // 335544372L;
    DEFINE_ECONSTANT(isc_sys_request)                      // 335544373L;
    DEFINE_ECONSTANT(isc_stream_eof)                       // 335544374L;
    DEFINE_ECONSTANT(isc_unavailable)                      // 335544375L;
    DEFINE_ECONSTANT(isc_unres_rel)                        // 335544376L;
    DEFINE_ECONSTANT(isc_uns_ext)                          // 335544377L;
    DEFINE_ECONSTANT(isc_wish_list)                        // 335544378L;
    DEFINE_ECONSTANT(isc_wrong_ods)                        // 335544379L;
    DEFINE_ECONSTANT(isc_wronumarg)                        // 335544380L;
    DEFINE_ECONSTANT(isc_imp_exc)                          // 335544381L;
    DEFINE_ECONSTANT(isc_random)                           // 335544382L;
    DEFINE_ECONSTANT(isc_fatal_conflict)                   // 335544383L;
    DEFINE_ECONSTANT(isc_badblk)                           // 335544384L;
    DEFINE_ECONSTANT(isc_invpoolcl)                        // 335544385L;
    DEFINE_ECONSTANT(isc_nopoolids)                        // 335544386L;
    DEFINE_ECONSTANT(isc_relbadblk)                        // 335544387L;
    DEFINE_ECONSTANT(isc_blktoobig)                        // 335544388L;
    DEFINE_ECONSTANT(isc_bufexh)                           // 335544389L;
    DEFINE_ECONSTANT(isc_syntaxerr)                        // 335544390L;
    DEFINE_ECONSTANT(isc_bufinuse)                         // 335544391L;
    DEFINE_ECONSTANT(isc_bdbincon)                         // 335544392L;
    DEFINE_ECONSTANT(isc_reqinuse)                         // 335544393L;
    DEFINE_ECONSTANT(isc_badodsver)                        // 335544394L;
    DEFINE_ECONSTANT(isc_relnotdef)                        // 335544395L;
    DEFINE_ECONSTANT(isc_fldnotdef)                        // 335544396L;
    DEFINE_ECONSTANT(isc_dirtypage)                        // 335544397L;
    DEFINE_ECONSTANT(isc_waifortra)                        // 335544398L;
    DEFINE_ECONSTANT(isc_doubleloc)                        // 335544399L;
    DEFINE_ECONSTANT(isc_nodnotfnd)                        // 335544400L;
    DEFINE_ECONSTANT(isc_dupnodfnd)                        // 335544401L;
    DEFINE_ECONSTANT(isc_locnotmar)                        // 335544402L;
    DEFINE_ECONSTANT(isc_badpagtyp)                        // 335544403L;
    DEFINE_ECONSTANT(isc_corrupt)                          // 335544404L;
    DEFINE_ECONSTANT(isc_badpage)                          // 335544405L;
    DEFINE_ECONSTANT(isc_badindex)                         // 335544406L;
    DEFINE_ECONSTANT(isc_dbbnotzer)                        // 335544407L;
    DEFINE_ECONSTANT(isc_tranotzer)                        // 335544408L;
    DEFINE_ECONSTANT(isc_trareqmis)                        // 335544409L;
    DEFINE_ECONSTANT(isc_badhndcnt)                        // 335544410L;
    DEFINE_ECONSTANT(isc_wrotpbver)                        // 335544411L;
    DEFINE_ECONSTANT(isc_wroblrver)                        // 335544412L;
    DEFINE_ECONSTANT(isc_wrodpbver)                        // 335544413L;
    DEFINE_ECONSTANT(isc_blobnotsup)                       // 335544414L;
    DEFINE_ECONSTANT(isc_badrelation)                      // 335544415L;
    DEFINE_ECONSTANT(isc_nodetach)                         // 335544416L;
    DEFINE_ECONSTANT(isc_notremote)                        // 335544417L;
    DEFINE_ECONSTANT(isc_trainlim)                         // 335544418L;
    DEFINE_ECONSTANT(isc_notinlim)                         // 335544419L;
    DEFINE_ECONSTANT(isc_traoutsta)                        // 335544420L;
    DEFINE_ECONSTANT(isc_connect_reject)                   // 335544421L;
    DEFINE_ECONSTANT(isc_dbfile)                           // 335544422L;
    DEFINE_ECONSTANT(isc_orphan)                           // 335544423L;
    DEFINE_ECONSTANT(isc_no_lock_mgr)                      // 335544424L;
    DEFINE_ECONSTANT(isc_ctxinuse)                         // 335544425L;
    DEFINE_ECONSTANT(isc_ctxnotdef)                        // 335544426L;
    DEFINE_ECONSTANT(isc_datnotsup)                        // 335544427L;
    DEFINE_ECONSTANT(isc_badmsgnum)                        // 335544428L;
    DEFINE_ECONSTANT(isc_badparnum)                        // 335544429L;
    DEFINE_ECONSTANT(isc_virmemexh)                        // 335544430L;
    DEFINE_ECONSTANT(isc_blocking_signal)                  // 335544431L;
    DEFINE_ECONSTANT(isc_lockmanerr)                       // 335544432L;
    DEFINE_ECONSTANT(isc_journerr)                         // 335544433L;
    DEFINE_ECONSTANT(isc_keytoobig)                        // 335544434L;
    DEFINE_ECONSTANT(isc_nullsegkey)                       // 335544435L;
    DEFINE_ECONSTANT(isc_sqlerr)                           // 335544436L;
    DEFINE_ECONSTANT(isc_wrodynver)                        // 335544437L;
    DEFINE_ECONSTANT(isc_funnotdef)                        // 335544438L;
    DEFINE_ECONSTANT(isc_funmismat)                        // 335544439L;
    DEFINE_ECONSTANT(isc_bad_msg_vec)                      // 335544440L;
    DEFINE_ECONSTANT(isc_bad_detach)                       // 335544441L;
    DEFINE_ECONSTANT(isc_noargacc_read)                    // 335544442L;
    DEFINE_ECONSTANT(isc_noargacc_write)                   // 335544443L;
    DEFINE_ECONSTANT(isc_read_only)                        // 335544444L;
    DEFINE_ECONSTANT(isc_ext_err)                          // 335544445L;
    DEFINE_ECONSTANT(isc_non_updatable)                    // 335544446L;
    DEFINE_ECONSTANT(isc_no_rollback)                      // 335544447L;
    DEFINE_ECONSTANT(isc_bad_sec_info)                     // 335544448L;
    DEFINE_ECONSTANT(isc_invalid_sec_info)                 // 335544449L;
    DEFINE_ECONSTANT(isc_misc_interpreted)                 // 335544450L;
    DEFINE_ECONSTANT(isc_update_conflict)                  // 335544451L;
    DEFINE_ECONSTANT(isc_unlicensed)                       // 335544452L;
    DEFINE_ECONSTANT(isc_obj_in_use)                       // 335544453L;
    DEFINE_ECONSTANT(isc_nofilter)                         // 335544454L;
    DEFINE_ECONSTANT(isc_shadow_accessed)                  // 335544455L;
    DEFINE_ECONSTANT(isc_invalid_sdl)                      // 335544456L;
    DEFINE_ECONSTANT(isc_out_of_bounds)                    // 335544457L;
    DEFINE_ECONSTANT(isc_invalid_dimension)                // 335544458L;
    DEFINE_ECONSTANT(isc_rec_in_limbo)                     // 335544459L;
    DEFINE_ECONSTANT(isc_shadow_missing)                   // 335544460L;
    DEFINE_ECONSTANT(isc_cant_validate)                    // 335544461L;
    DEFINE_ECONSTANT(isc_cant_start_journal)               // 335544462L;
    DEFINE_ECONSTANT(isc_gennotdef)                        // 335544463L;
    DEFINE_ECONSTANT(isc_cant_start_logging)               // 335544464L;
    DEFINE_ECONSTANT(isc_bad_segstr_type)                  // 335544465L;
    DEFINE_ECONSTANT(isc_foreign_key)                      // 335544466L;
    DEFINE_ECONSTANT(isc_high_minor)                       // 335544467L;
    DEFINE_ECONSTANT(isc_tra_state)                        // 335544468L;
    DEFINE_ECONSTANT(isc_trans_invalid)                    // 335544469L;
    DEFINE_ECONSTANT(isc_buf_invalid)                      // 335544470L;
    DEFINE_ECONSTANT(isc_indexnotdefined)                  // 335544471L;
    DEFINE_ECONSTANT(isc_login)                            // 335544472L;
    DEFINE_ECONSTANT(isc_invalid_bookmark)                 // 335544473L;
    DEFINE_ECONSTANT(isc_bad_lock_level)                   // 335544474L;
    DEFINE_ECONSTANT(isc_relation_lock)                    // 335544475L;
    DEFINE_ECONSTANT(isc_record_lock)                      // 335544476L;
    DEFINE_ECONSTANT(isc_max_idx)                          // 335544477L;
    DEFINE_ECONSTANT(isc_jrn_enable)                       // 335544478L;
    DEFINE_ECONSTANT(isc_old_failure)                      // 335544479L;
    DEFINE_ECONSTANT(isc_old_in_progress)                  // 335544480L;
    DEFINE_ECONSTANT(isc_old_no_space)                     // 335544481L;
    DEFINE_ECONSTANT(isc_no_wal_no_jrn)                    // 335544482L;
    DEFINE_ECONSTANT(isc_num_old_files)                    // 335544483L;
    DEFINE_ECONSTANT(isc_wal_file_open)                    // 335544484L;
    DEFINE_ECONSTANT(isc_bad_stmt_handle)                  // 335544485L;
    DEFINE_ECONSTANT(isc_wal_failure)                      // 335544486L;
    DEFINE_ECONSTANT(isc_walw_err)                         // 335544487L;
    DEFINE_ECONSTANT(isc_logh_small)                       // 335544488L;
    DEFINE_ECONSTANT(isc_logh_inv_version)                 // 335544489L;
    DEFINE_ECONSTANT(isc_logh_open_flag)                   // 335544490L;
    DEFINE_ECONSTANT(isc_logh_open_flag2)                  // 335544491L;
    DEFINE_ECONSTANT(isc_logh_diff_dbname)                 // 335544492L;
    DEFINE_ECONSTANT(isc_logf_unexpected_eof)              // 335544493L;
    DEFINE_ECONSTANT(isc_logr_incomplete)                  // 335544494L;
    DEFINE_ECONSTANT(isc_logr_header_small)                // 335544495L;
    DEFINE_ECONSTANT(isc_logb_small)                       // 335544496L;
    DEFINE_ECONSTANT(isc_wal_illegal_attach)               // 335544497L;
    DEFINE_ECONSTANT(isc_wal_invalid_wpb)                  // 335544498L;
    DEFINE_ECONSTANT(isc_wal_err_rollover)                 // 335544499L;
    DEFINE_ECONSTANT(isc_no_wal)                           // 335544500L;
    DEFINE_ECONSTANT(isc_drop_wal)                         // 335544501L;
    DEFINE_ECONSTANT(isc_stream_not_defined)               // 335544502L;
    DEFINE_ECONSTANT(isc_wal_subsys_error)                 // 335544503L;
    DEFINE_ECONSTANT(isc_wal_subsys_corrupt)               // 335544504L;
    DEFINE_ECONSTANT(isc_no_archive)                       // 335544505L;
    DEFINE_ECONSTANT(isc_shutinprog)                       // 335544506L;
    DEFINE_ECONSTANT(isc_range_in_use)                     // 335544507L;
    DEFINE_ECONSTANT(isc_range_not_found)                  // 335544508L;
    DEFINE_ECONSTANT(isc_charset_not_found)                // 335544509L;
    DEFINE_ECONSTANT(isc_lock_timeout)                     // 335544510L;
    DEFINE_ECONSTANT(isc_prcnotdef)                        // 335544511L;
    DEFINE_ECONSTANT(isc_prcmismat)                        // 335544512L;
    DEFINE_ECONSTANT(isc_wal_bugcheck)                     // 335544513L;
    DEFINE_ECONSTANT(isc_wal_cant_expand)                  // 335544514L;
    DEFINE_ECONSTANT(isc_codnotdef)                        // 335544515L;
    DEFINE_ECONSTANT(isc_xcpnotdef)                        // 335544516L;
    DEFINE_ECONSTANT(isc_except)                           // 335544517L;
    DEFINE_ECONSTANT(isc_cache_restart)                    // 335544518L;
    DEFINE_ECONSTANT(isc_bad_lock_handle)                  // 335544519L;
    DEFINE_ECONSTANT(isc_jrn_present)                      // 335544520L;
    DEFINE_ECONSTANT(isc_wal_err_rollover2)                // 335544521L;
    DEFINE_ECONSTANT(isc_wal_err_logwrite)                 // 335544522L;
    DEFINE_ECONSTANT(isc_wal_err_jrn_comm)                 // 335544523L;
    DEFINE_ECONSTANT(isc_wal_err_expansion)                // 335544524L;
    DEFINE_ECONSTANT(isc_wal_err_setup)                    // 335544525L;
    DEFINE_ECONSTANT(isc_wal_err_ww_sync)                  // 335544526L;
    DEFINE_ECONSTANT(isc_wal_err_ww_start)                 // 335544527L;
    DEFINE_ECONSTANT(isc_shutdown)                         // 335544528L;
    DEFINE_ECONSTANT(isc_existing_priv_mod)                // 335544529L;
    DEFINE_ECONSTANT(isc_primary_key_ref)                  // 335544530L;
    DEFINE_ECONSTANT(isc_primary_key_notnull)              // 335544531L;
    DEFINE_ECONSTANT(isc_ref_cnstrnt_notfound)             // 335544532L;
    DEFINE_ECONSTANT(isc_foreign_key_notfound)             // 335544533L;
    DEFINE_ECONSTANT(isc_ref_cnstrnt_update)               // 335544534L;
    DEFINE_ECONSTANT(isc_check_cnstrnt_update)             // 335544535L;
    DEFINE_ECONSTANT(isc_check_cnstrnt_del)                // 335544536L;
    DEFINE_ECONSTANT(isc_integ_index_seg_del)              // 335544537L;
    DEFINE_ECONSTANT(isc_integ_index_seg_mod)              // 335544538L;
    DEFINE_ECONSTANT(isc_integ_index_del)                  // 335544539L;
    DEFINE_ECONSTANT(isc_integ_index_mod)                  // 335544540L;
    DEFINE_ECONSTANT(isc_check_trig_del)                   // 335544541L;
    DEFINE_ECONSTANT(isc_check_trig_update)                // 335544542L;
    DEFINE_ECONSTANT(isc_cnstrnt_fld_del)                  // 335544543L;
    DEFINE_ECONSTANT(isc_cnstrnt_fld_rename)               // 335544544L;
    DEFINE_ECONSTANT(isc_rel_cnstrnt_update)               // 335544545L;
    DEFINE_ECONSTANT(isc_constaint_on_view)                // 335544546L;
    DEFINE_ECONSTANT(isc_invld_cnstrnt_type)               // 335544547L;
    DEFINE_ECONSTANT(isc_primary_key_exists)               // 335544548L;
    DEFINE_ECONSTANT(isc_systrig_update)                   // 335544549L;
    DEFINE_ECONSTANT(isc_not_rel_owner)                    // 335544550L;
    DEFINE_ECONSTANT(isc_grant_obj_notfound)               // 335544551L;
    DEFINE_ECONSTANT(isc_grant_fld_notfound)               // 335544552L;
    DEFINE_ECONSTANT(isc_grant_nopriv)                     // 335544553L;
    DEFINE_ECONSTANT(isc_nonsql_security_rel)              // 335544554L;
    DEFINE_ECONSTANT(isc_nonsql_security_fld)              // 335544555L;
    DEFINE_ECONSTANT(isc_wal_cache_err)                    // 335544556L;
    DEFINE_ECONSTANT(isc_shutfail)                         // 335544557L;
    DEFINE_ECONSTANT(isc_check_constraint)                 // 335544558L;
    DEFINE_ECONSTANT(isc_bad_svc_handle)                   // 335544559L;
    DEFINE_ECONSTANT(isc_shutwarn)                         // 335544560L;
    DEFINE_ECONSTANT(isc_wrospbver)                        // 335544561L;
    DEFINE_ECONSTANT(isc_bad_spb_form)                     // 335544562L;
    DEFINE_ECONSTANT(isc_svcnotdef)                        // 335544563L;
    DEFINE_ECONSTANT(isc_no_jrn)                           // 335544564L;
    DEFINE_ECONSTANT(isc_transliteration_failed)           // 335544565L;
    DEFINE_ECONSTANT(isc_start_cm_for_wal)                 // 335544566L;
    DEFINE_ECONSTANT(isc_wal_ovflow_log_required)          // 335544567L;
    DEFINE_ECONSTANT(isc_text_subtype)                     // 335544568L;
    DEFINE_ECONSTANT(isc_dsql_error)                       // 335544569L;
    DEFINE_ECONSTANT(isc_dsql_command_err)                 // 335544570L;
    DEFINE_ECONSTANT(isc_dsql_constant_err)                // 335544571L;
    DEFINE_ECONSTANT(isc_dsql_cursor_err)                  // 335544572L;
    DEFINE_ECONSTANT(isc_dsql_datatype_err)                // 335544573L;
    DEFINE_ECONSTANT(isc_dsql_decl_err)                    // 335544574L;
    DEFINE_ECONSTANT(isc_dsql_cursor_update_err)           // 335544575L;
    DEFINE_ECONSTANT(isc_dsql_cursor_open_err)             // 335544576L;
    DEFINE_ECONSTANT(isc_dsql_cursor_close_err)            // 335544577L;
    DEFINE_ECONSTANT(isc_dsql_field_err)                   // 335544578L;
    DEFINE_ECONSTANT(isc_dsql_internal_err)                // 335544579L;
    DEFINE_ECONSTANT(isc_dsql_relation_err)                // 335544580L;
    DEFINE_ECONSTANT(isc_dsql_procedure_err)               // 335544581L;
    DEFINE_ECONSTANT(isc_dsql_request_err)                 // 335544582L;
    DEFINE_ECONSTANT(isc_dsql_sqlda_err)                   // 335544583L;
    DEFINE_ECONSTANT(isc_dsql_var_count_err)               // 335544584L;
    DEFINE_ECONSTANT(isc_dsql_stmt_handle)                 // 335544585L;
    DEFINE_ECONSTANT(isc_dsql_function_err)                // 335544586L;
    DEFINE_ECONSTANT(isc_dsql_blob_err)                    // 335544587L;
    DEFINE_ECONSTANT(isc_collation_not_found)              // 335544588L;
    DEFINE_ECONSTANT(isc_collation_not_for_charset)        // 335544589L;
    DEFINE_ECONSTANT(isc_dsql_dup_option)                  // 335544590L;
    DEFINE_ECONSTANT(isc_dsql_tran_err)                    // 335544591L;
    DEFINE_ECONSTANT(isc_dsql_invalid_array)               // 335544592L;
    DEFINE_ECONSTANT(isc_dsql_max_arr_dim_exceeded)        // 335544593L;
    DEFINE_ECONSTANT(isc_dsql_arr_range_error)             // 335544594L;
    DEFINE_ECONSTANT(isc_dsql_trigger_err)                 // 335544595L;
    DEFINE_ECONSTANT(isc_dsql_subselect_err)               // 335544596L;
    DEFINE_ECONSTANT(isc_dsql_crdb_prepare_err)            // 335544597L;
    DEFINE_ECONSTANT(isc_specify_field_err)                // 335544598L;
    DEFINE_ECONSTANT(isc_num_field_err)                    // 335544599L;
    DEFINE_ECONSTANT(isc_col_name_err)                     // 335544600L;
    DEFINE_ECONSTANT(isc_where_err)                        // 335544601L;
    DEFINE_ECONSTANT(isc_table_view_err)                   // 335544602L;
    DEFINE_ECONSTANT(isc_distinct_err)                     // 335544603L;
    DEFINE_ECONSTANT(isc_key_field_count_err)              // 335544604L;
    DEFINE_ECONSTANT(isc_subquery_err)                     // 335544605L;
    DEFINE_ECONSTANT(isc_expression_eval_err)              // 335544606L;
    DEFINE_ECONSTANT(isc_node_err)                         // 335544607L;
    DEFINE_ECONSTANT(isc_command_end_err)                  // 335544608L;
    DEFINE_ECONSTANT(isc_index_name)                       // 335544609L;
    DEFINE_ECONSTANT(isc_exception_name)                   // 335544610L;
    DEFINE_ECONSTANT(isc_field_name)                       // 335544611L;
    DEFINE_ECONSTANT(isc_token_err)                        // 335544612L;
    DEFINE_ECONSTANT(isc_union_err)                        // 335544613L;
    DEFINE_ECONSTANT(isc_dsql_construct_err)               // 335544614L;
    DEFINE_ECONSTANT(isc_field_aggregate_err)              // 335544615L;
    DEFINE_ECONSTANT(isc_field_ref_err)                    // 335544616L;
    DEFINE_ECONSTANT(isc_order_by_err)                     // 335544617L;
    DEFINE_ECONSTANT(isc_return_mode_err)                  // 335544618L;
    DEFINE_ECONSTANT(isc_extern_func_err)                  // 335544619L;
    DEFINE_ECONSTANT(isc_alias_conflict_err)               // 335544620L;
    DEFINE_ECONSTANT(isc_procedure_conflict_error)         // 335544621L;
    DEFINE_ECONSTANT(isc_relation_conflict_err)            // 335544622L;
    DEFINE_ECONSTANT(isc_dsql_domain_err)                  // 335544623L;
    DEFINE_ECONSTANT(isc_idx_seg_err)                      // 335544624L;
    DEFINE_ECONSTANT(isc_node_name_err)                    // 335544625L;
    DEFINE_ECONSTANT(isc_table_name)                       // 335544626L;
    DEFINE_ECONSTANT(isc_proc_name)                        // 335544627L;
    DEFINE_ECONSTANT(isc_idx_create_err)                   // 335544628L;
    DEFINE_ECONSTANT(isc_wal_shadow_err)                   // 335544629L;
    DEFINE_ECONSTANT(isc_dependency)                       // 335544630L;
    DEFINE_ECONSTANT(isc_idx_key_err)                      // 335544631L;
    DEFINE_ECONSTANT(isc_dsql_file_length_err)             // 335544632L;
    DEFINE_ECONSTANT(isc_dsql_shadow_number_err)           // 335544633L;
    DEFINE_ECONSTANT(isc_dsql_token_unk_err)               // 335544634L;
    DEFINE_ECONSTANT(isc_dsql_no_relation_alias)           // 335544635L;
    DEFINE_ECONSTANT(isc_indexname)                        // 335544636L;
    DEFINE_ECONSTANT(isc_no_stream_plan)                   // 335544637L;
    DEFINE_ECONSTANT(isc_stream_twice)                     // 335544638L;
    DEFINE_ECONSTANT(isc_stream_not_found)                 // 335544639L;
    DEFINE_ECONSTANT(isc_collation_requires_text)          // 335544640L;
    DEFINE_ECONSTANT(isc_dsql_domain_not_found)            // 335544641L;
    DEFINE_ECONSTANT(isc_index_unused)                     // 335544642L;
    DEFINE_ECONSTANT(isc_dsql_self_join)                   // 335544643L;
    DEFINE_ECONSTANT(isc_stream_bof)                       // 335544644L;
    DEFINE_ECONSTANT(isc_stream_crack)                     // 335544645L;
    DEFINE_ECONSTANT(isc_db_or_file_exists)                // 335544646L;
    DEFINE_ECONSTANT(isc_invalid_operator)                 // 335544647L;
    DEFINE_ECONSTANT(isc_conn_lost)                        // 335544648L;
    DEFINE_ECONSTANT(isc_bad_checksum)                     // 335544649L;
    DEFINE_ECONSTANT(isc_page_type_err)                    // 335544650L;
    DEFINE_ECONSTANT(isc_ext_readonly_err)                 // 335544651L;
    DEFINE_ECONSTANT(isc_sing_select_err)                  // 335544652L;
    DEFINE_ECONSTANT(isc_psw_attach)                       // 335544653L;
    DEFINE_ECONSTANT(isc_psw_start_trans)                  // 335544654L;
    DEFINE_ECONSTANT(isc_invalid_direction)                // 335544655L;
    DEFINE_ECONSTANT(isc_dsql_var_conflict)                // 335544656L;
    DEFINE_ECONSTANT(isc_dsql_no_blob_array)               // 335544657L;
    DEFINE_ECONSTANT(isc_dsql_base_table)                  // 335544658L;
    DEFINE_ECONSTANT(isc_duplicate_base_table)             // 335544659L;
    DEFINE_ECONSTANT(isc_view_alias)                       // 335544660L;
    DEFINE_ECONSTANT(isc_index_root_page_full)             // 335544661L;
    DEFINE_ECONSTANT(isc_dsql_blob_type_unknown)           // 335544662L;
    DEFINE_ECONSTANT(isc_req_max_clones_exceeded)          // 335544663L;
    DEFINE_ECONSTANT(isc_dsql_duplicate_spec)              // 335544664L;
    DEFINE_ECONSTANT(isc_unique_key_violation)             // 335544665L;
    DEFINE_ECONSTANT(isc_srvr_version_too_old)             // 335544666L;
    DEFINE_ECONSTANT(isc_drdb_completed_with_errs)         // 335544667L;
    DEFINE_ECONSTANT(isc_dsql_procedure_use_err)           // 335544668L;
    DEFINE_ECONSTANT(isc_dsql_count_mismatch)              // 335544669L;
    DEFINE_ECONSTANT(isc_blob_idx_err)                     // 335544670L;
    DEFINE_ECONSTANT(isc_array_idx_err)                    // 335544671L;
    DEFINE_ECONSTANT(isc_key_field_err)                    // 335544672L;
    DEFINE_ECONSTANT(isc_no_delete)                        // 335544673L;
    DEFINE_ECONSTANT(isc_del_last_field)                   // 335544674L;
    DEFINE_ECONSTANT(isc_sort_err)                         // 335544675L;
    DEFINE_ECONSTANT(isc_sort_mem_err)                     // 335544676L;
    DEFINE_ECONSTANT(isc_version_err)                      // 335544677L;
    DEFINE_ECONSTANT(isc_inval_key_posn)                   // 335544678L;
    DEFINE_ECONSTANT(isc_no_segments_err)                  // 335544679L;
    DEFINE_ECONSTANT(isc_crrp_data_err)                    // 335544680L;
    DEFINE_ECONSTANT(isc_rec_size_err)                     // 335544681L;
    DEFINE_ECONSTANT(isc_dsql_field_ref)                   // 335544682L;
    DEFINE_ECONSTANT(isc_req_depth_exceeded)               // 335544683L;
    DEFINE_ECONSTANT(isc_no_field_access)                  // 335544684L;
    DEFINE_ECONSTANT(isc_no_dbkey)                         // 335544685L;
    DEFINE_ECONSTANT(isc_jrn_format_err)                   // 335544686L;
    DEFINE_ECONSTANT(isc_jrn_file_full)                    // 335544687L;
    DEFINE_ECONSTANT(isc_dsql_open_cursor_request)         // 335544688L;
    DEFINE_ECONSTANT(isc_ib_error)                         // 335544689L;
    DEFINE_ECONSTANT(isc_cache_redef)                      // 335544690L;
    DEFINE_ECONSTANT(isc_cache_too_small)                  // 335544691L;
    DEFINE_ECONSTANT(isc_log_redef)                        // 335544692L;
    DEFINE_ECONSTANT(isc_log_too_small)                    // 335544693L;
    DEFINE_ECONSTANT(isc_partition_too_small)              // 335544694L;
    DEFINE_ECONSTANT(isc_partition_not_supp)               // 335544695L;
    DEFINE_ECONSTANT(isc_log_length_spec)                  // 335544696L;
    DEFINE_ECONSTANT(isc_precision_err)                    // 335544697L;
    DEFINE_ECONSTANT(isc_scale_nogt)                       // 335544698L;
    DEFINE_ECONSTANT(isc_expec_short)                      // 335544699L;
    DEFINE_ECONSTANT(isc_expec_long)                       // 335544700L;
    DEFINE_ECONSTANT(isc_expec_ushort)                     // 335544701L;
    //DEFINE_ECONSTANT(isc_like_escape_invalid) // 335544702L;
    DEFINE_ECONSTANT(isc_svcnoexe)                         // 335544703L;
    DEFINE_ECONSTANT(isc_net_lookup_err)                   // 335544704L;
    DEFINE_ECONSTANT(isc_service_unknown)                  // 335544705L;
    DEFINE_ECONSTANT(isc_host_unknown)                     // 335544706L;
    DEFINE_ECONSTANT(isc_grant_nopriv_on_base)             // 335544707L;
    DEFINE_ECONSTANT(isc_dyn_fld_ambiguous)                // 335544708L;
    DEFINE_ECONSTANT(isc_dsql_agg_ref_err)                 // 335544709L;
    DEFINE_ECONSTANT(isc_complex_view)                     // 335544710L;
    DEFINE_ECONSTANT(isc_unprepared_stmt)                  // 335544711L;
    DEFINE_ECONSTANT(isc_expec_positive)                   // 335544712L;
    DEFINE_ECONSTANT(isc_dsql_sqlda_value_err)             // 335544713L;
    DEFINE_ECONSTANT(isc_invalid_array_id)                 // 335544714L;
    DEFINE_ECONSTANT(isc_extfile_uns_op)                   // 335544715L;
    DEFINE_ECONSTANT(isc_svc_in_use)                       // 335544716L;
    DEFINE_ECONSTANT(isc_err_stack_limit)                  // 335544717L;
    DEFINE_ECONSTANT(isc_invalid_key)                      // 335544718L;
    DEFINE_ECONSTANT(isc_net_init_error)                   // 335544719L;
    DEFINE_ECONSTANT(isc_loadlib_failure)                  // 335544720L;
    DEFINE_ECONSTANT(isc_network_error)                    // 335544721L;
    DEFINE_ECONSTANT(isc_net_connect_err)                  // 335544722L;
    DEFINE_ECONSTANT(isc_net_connect_listen_err)           // 335544723L;
    DEFINE_ECONSTANT(isc_net_event_connect_err)            // 335544724L;
    DEFINE_ECONSTANT(isc_net_event_listen_err)             // 335544725L;
    DEFINE_ECONSTANT(isc_net_read_err)                     // 335544726L;
    DEFINE_ECONSTANT(isc_net_write_err)                    // 335544727L;
    DEFINE_ECONSTANT(isc_integ_index_deactivate)           // 335544728L;
    DEFINE_ECONSTANT(isc_integ_deactivate_primary)         // 335544729L;
    DEFINE_ECONSTANT(isc_cse_not_supported)                // 335544730L;
    DEFINE_ECONSTANT(isc_tra_must_sweep)                   // 335544731L;
    DEFINE_ECONSTANT(isc_unsupported_network_drive)        // 335544732L;
    DEFINE_ECONSTANT(isc_io_create_err)                    // 335544733L;
    DEFINE_ECONSTANT(isc_io_open_err)                      // 335544734L;
    DEFINE_ECONSTANT(isc_io_close_err)                     // 335544735L;
    DEFINE_ECONSTANT(isc_io_read_err)                      // 335544736L;
    DEFINE_ECONSTANT(isc_io_write_err)                     // 335544737L;
    DEFINE_ECONSTANT(isc_io_delete_err)                    // 335544738L;
    DEFINE_ECONSTANT(isc_io_access_err)                    // 335544739L;
    DEFINE_ECONSTANT(isc_udf_exception)                    // 335544740L;
    DEFINE_ECONSTANT(isc_lost_db_connection)               // 335544741L;
    DEFINE_ECONSTANT(isc_no_write_user_priv)               // 335544742L;
    DEFINE_ECONSTANT(isc_token_too_long)                   // 335544743L;
    DEFINE_ECONSTANT(isc_max_att_exceeded)                 // 335544744L;
    DEFINE_ECONSTANT(isc_login_same_as_role_name)          // 335544745L;
    DEFINE_ECONSTANT(isc_reftable_requires_pk)             // 335544746L;
    DEFINE_ECONSTANT(isc_usrname_too_long)                 // 335544747L;
    DEFINE_ECONSTANT(isc_password_too_long)                // 335544748L;
    DEFINE_ECONSTANT(isc_usrname_required)                 // 335544749L;
    DEFINE_ECONSTANT(isc_password_required)                // 335544750L;
    DEFINE_ECONSTANT(isc_bad_protocol)                     // 335544751L;
    DEFINE_ECONSTANT(isc_dup_usrname_found)                // 335544752L;
    DEFINE_ECONSTANT(isc_usrname_not_found)                // 335544753L;
    DEFINE_ECONSTANT(isc_error_adding_sec_record)          // 335544754L;
    DEFINE_ECONSTANT(isc_error_modifying_sec_record)       // 335544755L;
    DEFINE_ECONSTANT(isc_error_deleting_sec_record)        // 335544756L;
    DEFINE_ECONSTANT(isc_error_updating_sec_db)            // 335544757L;
    DEFINE_ECONSTANT(isc_sort_rec_size_err)                // 335544758L;
    DEFINE_ECONSTANT(isc_bad_default_value)                // 335544759L;
    DEFINE_ECONSTANT(isc_invalid_clause)                   // 335544760L;
    DEFINE_ECONSTANT(isc_too_many_handles)                 // 335544761L;
    DEFINE_ECONSTANT(isc_optimizer_blk_exc)                // 335544762L;
    DEFINE_ECONSTANT(isc_invalid_string_constant)          // 335544763L;
    DEFINE_ECONSTANT(isc_transitional_date)                // 335544764L;
    DEFINE_ECONSTANT(isc_read_only_database)               // 335544765L;
    DEFINE_ECONSTANT(isc_must_be_dialect_2_and_up)         // 335544766L;
    DEFINE_ECONSTANT(isc_blob_filter_exception)            // 335544767L;
    DEFINE_ECONSTANT(isc_exception_access_violation)       // 335544768L;
    DEFINE_ECONSTANT(isc_exception_datatype_missalignment) // 335544769L;
    DEFINE_ECONSTANT(isc_exception_array_bounds_exceeded)  // 335544770L;
    DEFINE_ECONSTANT(isc_exception_float_denormal_operand) // 335544771L;
    DEFINE_ECONSTANT(isc_exception_float_divide_by_zero)   // 335544772L;
    DEFINE_ECONSTANT(isc_exception_float_inexact_result)   // 335544773L;
    DEFINE_ECONSTANT(isc_exception_float_invalid_operand)  // 335544774L;
    DEFINE_ECONSTANT(isc_exception_float_overflow)         // 335544775L;
    DEFINE_ECONSTANT(isc_exception_float_stack_check)      // 335544776L;
    DEFINE_ECONSTANT(isc_exception_float_underflow)        // 335544777L;
    DEFINE_ECONSTANT(isc_exception_integer_divide_by_zero) // 335544778L;
    DEFINE_ECONSTANT(isc_exception_integer_overflow)       // 335544779L;
    DEFINE_ECONSTANT(isc_exception_unknown)                // 335544780L;
    DEFINE_ECONSTANT(isc_exception_stack_overflow)         // 335544781L;
    DEFINE_ECONSTANT(isc_exception_sigsegv)                // 335544782L;
    DEFINE_ECONSTANT(isc_exception_sigill)                 // 335544783L;
    DEFINE_ECONSTANT(isc_exception_sigbus)                 // 335544784L;
    DEFINE_ECONSTANT(isc_exception_sigfpe)                 // 335544785L;
    DEFINE_ECONSTANT(isc_ext_file_delete)                  // 335544786L;
    DEFINE_ECONSTANT(isc_ext_file_modify)                  // 335544787L;
    DEFINE_ECONSTANT(isc_adm_task_denied)                  // 335544788L;
    DEFINE_ECONSTANT(isc_extract_input_mismatch)           // 335544789L;
    DEFINE_ECONSTANT(isc_insufficient_svc_privileges)      // 335544790L;
    DEFINE_ECONSTANT(isc_file_in_use)                      // 335544791L;
    DEFINE_ECONSTANT(isc_service_att_err)                  // 335544792L;
    DEFINE_ECONSTANT(isc_ddl_not_allowed_by_db_sql_dial)   // 335544793L;
    DEFINE_ECONSTANT(isc_cancelled)                        // 335544794L;
    DEFINE_ECONSTANT(isc_unexp_spb_form)                   // 335544795L;
    DEFINE_ECONSTANT(isc_sql_dialect_datatype_unsupport)   // 335544796L;
    DEFINE_ECONSTANT(isc_svcnouser)                        // 335544797L;
    DEFINE_ECONSTANT(isc_depend_on_uncommitted_rel)        // 335544798L;
    DEFINE_ECONSTANT(isc_svc_name_missing)                 // 335544799L;
    DEFINE_ECONSTANT(isc_too_many_contexts)                // 335544800L;
    DEFINE_ECONSTANT(isc_datype_notsup)                    // 335544801L;
    DEFINE_ECONSTANT(isc_dialect_reset_warning)            // 335544802L;
    DEFINE_ECONSTANT(isc_dialect_not_changed)              // 335544803L;
    DEFINE_ECONSTANT(isc_database_create_failed)           // 335544804L;
    DEFINE_ECONSTANT(isc_inv_dialect_specified)            // 335544805L;
    DEFINE_ECONSTANT(isc_valid_db_dialects)                // 335544806L;
    DEFINE_ECONSTANT(isc_sqlwarn)                          // 335544807L;
    DEFINE_ECONSTANT(isc_dtype_renamed)                    // 335544808L;
    DEFINE_ECONSTANT(isc_extern_func_dir_error)            // 335544809L;
    DEFINE_ECONSTANT(isc_date_range_exceeded)              // 335544810L;
    DEFINE_ECONSTANT(isc_inv_client_dialect_specified)     // 335544811L;
    DEFINE_ECONSTANT(isc_valid_client_dialects)            // 335544812L;
    DEFINE_ECONSTANT(isc_optimizer_between_err)            // 335544813L;
    DEFINE_ECONSTANT(isc_service_not_supported)            // 335544814L;
    DEFINE_ECONSTANT(isc_generator_name)                   // 335544815L;
    DEFINE_ECONSTANT(isc_udf_name)                         // 335544816L;
    DEFINE_ECONSTANT(isc_bad_limit_param)                  // 335544817L;
    DEFINE_ECONSTANT(isc_bad_skip_param)                   // 335544818L;
    DEFINE_ECONSTANT(isc_io_32bit_exceeded_err)            // 335544819L;
    DEFINE_ECONSTANT(isc_invalid_savepoint)                // 335544820L;
    DEFINE_ECONSTANT(isc_dsql_column_pos_err)              // 335544821L;
    DEFINE_ECONSTANT(isc_dsql_agg_where_err)               // 335544822L;
    DEFINE_ECONSTANT(isc_dsql_agg_group_err)               // 335544823L;
    DEFINE_ECONSTANT(isc_dsql_agg_column_err)              // 335544824L;
    DEFINE_ECONSTANT(isc_dsql_agg_having_err)              // 335544825L;
    DEFINE_ECONSTANT(isc_dsql_agg_nested_err)              // 335544826L;
    DEFINE_ECONSTANT(isc_exec_sql_invalid_arg)             // 335544827L;
    DEFINE_ECONSTANT(isc_exec_sql_invalid_req)             // 335544828L;
    DEFINE_ECONSTANT(isc_exec_sql_invalid_var)             // 335544829L;
    DEFINE_ECONSTANT(isc_exec_sql_max_call_exceeded)       // 335544830L;
    DEFINE_ECONSTANT(isc_conf_access_denied)               // 335544831L;
    DEFINE_ECONSTANT(isc_wrong_backup_state)               // 335544832L;
    DEFINE_ECONSTANT(isc_wal_backup_err)                   // 335544833L;
    DEFINE_ECONSTANT(isc_cursor_not_open)                  // 335544834L;
    DEFINE_ECONSTANT(isc_bad_shutdown_mode)                // 335544835L;
    DEFINE_ECONSTANT(isc_concat_overflow)                  // 335544836L;
    DEFINE_ECONSTANT(isc_bad_substring_offset)             // 335544837L;
    DEFINE_ECONSTANT(isc_foreign_key_target_doesnt_exist)  // 335544838L;
    DEFINE_ECONSTANT(isc_foreign_key_references_present)   // 335544839L;
    DEFINE_ECONSTANT(isc_no_update)                        // 335544840L;
    DEFINE_ECONSTANT(isc_cursor_already_open)              // 335544841L;
    DEFINE_ECONSTANT(isc_stack_trace)                      // 335544842L;
    DEFINE_ECONSTANT(isc_ctx_var_not_found)                // 335544843L;
    DEFINE_ECONSTANT(isc_ctx_namespace_invalid)            // 335544844L;
    DEFINE_ECONSTANT(isc_ctx_too_big)                      // 335544845L;
    DEFINE_ECONSTANT(isc_ctx_bad_argument)                 // 335544846L;
    DEFINE_ECONSTANT(isc_identifier_too_long)              // 335544847L;
    DEFINE_ECONSTANT(isc_except2)                          // 335544848L;
    DEFINE_ECONSTANT(isc_malformed_string)                 // 335544849L;
    DEFINE_ECONSTANT(isc_prc_out_param_mismatch)           // 335544850L;
    DEFINE_ECONSTANT(isc_command_end_err2)                 // 335544851L;
    DEFINE_ECONSTANT(isc_partner_idx_incompat_type)        // 335544852L;
    DEFINE_ECONSTANT(isc_bad_substring_length)             // 335544853L;
    DEFINE_ECONSTANT(isc_charset_not_installed)            // 335544854L;
    DEFINE_ECONSTANT(isc_collation_not_installed)          // 335544855L;
    DEFINE_ECONSTANT(isc_att_shutdown)                     // 335544856L;
    DEFINE_ECONSTANT(isc_blobtoobig)                       // 335544857L;
    DEFINE_ECONSTANT(isc_must_have_phys_field)             // 335544858L;
    DEFINE_ECONSTANT(isc_invalid_time_precision)           // 335544859L;
    DEFINE_ECONSTANT(isc_blob_convert_error)               // 335544860L;
    DEFINE_ECONSTANT(isc_array_convert_error)              // 335544861L;
    DEFINE_ECONSTANT(isc_record_lock_not_supp)             // 335544862L;
    DEFINE_ECONSTANT(isc_partner_idx_not_found)            // 335544863L;
    DEFINE_ECONSTANT(isc_tra_num_exc)                      // 335544864L;
    DEFINE_ECONSTANT(isc_field_disappeared)                // 335544865L;
    DEFINE_ECONSTANT(isc_met_wrong_gtt_scope)              // 335544866L;
    DEFINE_ECONSTANT(isc_subtype_for_internal_use)         // 335544867L;
    DEFINE_ECONSTANT(isc_illegal_prc_type)                 // 335544868L;
    DEFINE_ECONSTANT(isc_invalid_sort_datatype)            // 335544869L;
    DEFINE_ECONSTANT(isc_collation_name)                   // 335544870L;
    DEFINE_ECONSTANT(isc_domain_name)                      // 335544871L;
    DEFINE_ECONSTANT(isc_domnotdef)                        // 335544872L;
    DEFINE_ECONSTANT(isc_array_max_dimensions)             // 335544873L;
    DEFINE_ECONSTANT(isc_max_db_per_trans_allowed)         // 335544874L;
    DEFINE_ECONSTANT(isc_bad_debug_format)                 // 335544875L;
    DEFINE_ECONSTANT(isc_bad_proc_BLR)                     // 335544876L;
    DEFINE_ECONSTANT(isc_key_too_big)                      // 335544877L;
    DEFINE_ECONSTANT(isc_concurrent_transaction)           // 335544878L;
    DEFINE_ECONSTANT(isc_not_valid_for_var)                // 335544879L;
    DEFINE_ECONSTANT(isc_not_valid_for)                    // 335544880L;
    DEFINE_ECONSTANT(isc_need_difference)                  // 335544881L;
    DEFINE_ECONSTANT(isc_long_login)                       // 335544882L;
    DEFINE_ECONSTANT(isc_fldnotdef2)                       // 335544883L;
    DEFINE_ECONSTANT(isc_gfix_db_name)                     // 335740929L;
    DEFINE_ECONSTANT(isc_gfix_invalid_sw)                  // 335740930L;
    DEFINE_ECONSTANT(isc_gfix_incmp_sw)                    // 335740932L;
    DEFINE_ECONSTANT(isc_gfix_replay_req)                  // 335740933L;
    DEFINE_ECONSTANT(isc_gfix_pgbuf_req)                   // 335740934L;
    DEFINE_ECONSTANT(isc_gfix_val_req)                     // 335740935L;
    DEFINE_ECONSTANT(isc_gfix_pval_req)                    // 335740936L;
    DEFINE_ECONSTANT(isc_gfix_trn_req)                     // 335740937L;
    DEFINE_ECONSTANT(isc_gfix_full_req)                    // 335740940L;
    DEFINE_ECONSTANT(isc_gfix_usrname_req)                 // 335740941L;
    DEFINE_ECONSTANT(isc_gfix_pass_req)                    // 335740942L;
    DEFINE_ECONSTANT(isc_gfix_subs_name)                   // 335740943L;
    DEFINE_ECONSTANT(isc_gfix_wal_req)                     // 335740944L;
    DEFINE_ECONSTANT(isc_gfix_sec_req)                     // 335740945L;
    DEFINE_ECONSTANT(isc_gfix_nval_req)                    // 335740946L;
    DEFINE_ECONSTANT(isc_gfix_type_shut)                   // 335740947L;
    DEFINE_ECONSTANT(isc_gfix_retry)                       // 335740948L;
    DEFINE_ECONSTANT(isc_gfix_retry_db)                    // 335740951L;
    DEFINE_ECONSTANT(isc_gfix_exceed_max)                  // 335740991L;
    DEFINE_ECONSTANT(isc_gfix_corrupt_pool)                // 335740992L;
    DEFINE_ECONSTANT(isc_gfix_mem_exhausted)               // 335740993L;
    DEFINE_ECONSTANT(isc_gfix_bad_pool)                    // 335740994L;
    DEFINE_ECONSTANT(isc_gfix_trn_not_valid)               // 335740995L;
    DEFINE_ECONSTANT(isc_gfix_unexp_eoi)                   // 335741012L;
    DEFINE_ECONSTANT(isc_gfix_recon_fail)                  // 335741018L;
    DEFINE_ECONSTANT(isc_gfix_trn_unknown)                 // 335741036L;
    DEFINE_ECONSTANT(isc_gfix_mode_req)                    // 335741038L;
    //DEFINE_ECONSTANT(isc_gfix_opt_SQL_dialect) // 335741039L;
    DEFINE_ECONSTANT(isc_gfix_pzval_req)                   // 335741042L;
    DEFINE_ECONSTANT(isc_dsql_dbkey_from_non_table)        // 336003074L;
    DEFINE_ECONSTANT(isc_dsql_transitional_numeric)        // 336003075L;
    DEFINE_ECONSTANT(isc_dsql_dialect_warning_expr)        // 336003076L;
    DEFINE_ECONSTANT(isc_sql_db_dialect_dtype_unsupport)   // 336003077L;
    DEFINE_ECONSTANT(isc_isc_sql_dialect_conflict_num)     // 336003079L;
    DEFINE_ECONSTANT(isc_dsql_warning_number_ambiguous)    // 336003080L;
    DEFINE_ECONSTANT(isc_dsql_warning_number_ambiguous1)   // 336003081L;
    DEFINE_ECONSTANT(isc_dsql_warn_precision_ambiguous)    // 336003082L;
    DEFINE_ECONSTANT(isc_dsql_warn_precision_ambiguous1)   // 336003083L;
    DEFINE_ECONSTANT(isc_dsql_warn_precision_ambiguous2)   // 336003084L;
    DEFINE_ECONSTANT(isc_dsql_ambiguous_field_name)        // 336003085L;
    DEFINE_ECONSTANT(isc_dsql_udf_return_pos_err)          // 336003086L;
    DEFINE_ECONSTANT(isc_dsql_invalid_label)               // 336003087L;
    DEFINE_ECONSTANT(isc_dsql_datatypes_not_comparable)    // 336003088L;
    DEFINE_ECONSTANT(isc_dsql_cursor_invalid)              // 336003089L;
    DEFINE_ECONSTANT(isc_dsql_cursor_redefined)            // 336003090L;
    DEFINE_ECONSTANT(isc_dsql_cursor_not_found)            // 336003091L;
    DEFINE_ECONSTANT(isc_dsql_cursor_exists)               // 336003092L;
    DEFINE_ECONSTANT(isc_dsql_cursor_rel_ambiguous)        // 336003093L;
    DEFINE_ECONSTANT(isc_dsql_cursor_rel_not_found)        // 336003094L;
    DEFINE_ECONSTANT(isc_dsql_cursor_not_open)             // 336003095L;
    DEFINE_ECONSTANT(isc_dsql_type_not_supp_ext_tab)       // 336003096L;
    DEFINE_ECONSTANT(isc_dsql_feature_not_supported_ods)   // 336003097L;
    DEFINE_ECONSTANT(isc_primary_key_required)             // 336003098L;
    DEFINE_ECONSTANT(isc_upd_ins_doesnt_match_pk)          // 336003099L;
    DEFINE_ECONSTANT(isc_upd_ins_doesnt_match_matching)    // 336003100L;
    DEFINE_ECONSTANT(isc_upd_ins_with_complex_view)        // 336003101L;
    DEFINE_ECONSTANT(isc_dsql_incompatible_trigger_type)   // 336003102L;
    DEFINE_ECONSTANT(isc_dsql_db_trigger_type_cant_change) // 336003103L;
    DEFINE_ECONSTANT(isc_dyn_role_does_not_exist)          // 336068796L;
    DEFINE_ECONSTANT(isc_dyn_no_grant_admin_opt)           // 336068797L;
    DEFINE_ECONSTANT(isc_dyn_user_not_role_member)         // 336068798L;
    DEFINE_ECONSTANT(isc_dyn_delete_role_failed)           // 336068799L;
    DEFINE_ECONSTANT(isc_dyn_grant_role_to_user)           // 336068800L;
    DEFINE_ECONSTANT(isc_dyn_inv_sql_role_name)            // 336068801L;
    DEFINE_ECONSTANT(isc_dyn_dup_sql_role)                 // 336068802L;
    DEFINE_ECONSTANT(isc_dyn_kywd_spec_for_role)           // 336068803L;
    DEFINE_ECONSTANT(isc_dyn_roles_not_supported)          // 336068804L;
    DEFINE_ECONSTANT(isc_dyn_domain_name_exists)           // 336068812L;
    DEFINE_ECONSTANT(isc_dyn_field_name_exists)            // 336068813L;
    DEFINE_ECONSTANT(isc_dyn_dependency_exists)            // 336068814L;
    DEFINE_ECONSTANT(isc_dyn_dtype_invalid)                // 336068815L;
    DEFINE_ECONSTANT(isc_dyn_char_fld_too_small)           // 336068816L;
    DEFINE_ECONSTANT(isc_dyn_invalid_dtype_conversion)     // 336068817L;
    DEFINE_ECONSTANT(isc_dyn_dtype_conv_invalid)           // 336068818L;
    DEFINE_ECONSTANT(isc_dyn_zero_len_id)                  // 336068820L;
    DEFINE_ECONSTANT(isc_max_coll_per_charset)             // 336068829L;
    DEFINE_ECONSTANT(isc_invalid_coll_attr)                // 336068830L;
    DEFINE_ECONSTANT(isc_dyn_wrong_gtt_scope)              // 336068840L;
    DEFINE_ECONSTANT(isc_dyn_scale_too_big)                // 336068852L;
    DEFINE_ECONSTANT(isc_dyn_precision_too_small)          // 336068853L;
    DEFINE_ECONSTANT(isc_dyn_miss_priv_warning)            // 336068855L;
    DEFINE_ECONSTANT(isc_dyn_ods_not_supp_feature)         // 336068856L;
    DEFINE_ECONSTANT(isc_gbak_unknown_switch)              // 336330753L;
    DEFINE_ECONSTANT(isc_gbak_page_size_missing)           // 336330754L;
    DEFINE_ECONSTANT(isc_gbak_page_size_toobig)            // 336330755L;
    DEFINE_ECONSTANT(isc_gbak_redir_ouput_missing)         // 336330756L;
    DEFINE_ECONSTANT(isc_gbak_switches_conflict)           // 336330757L;
    DEFINE_ECONSTANT(isc_gbak_unknown_device)              // 336330758L;
    DEFINE_ECONSTANT(isc_gbak_no_protection)               // 336330759L;
    DEFINE_ECONSTANT(isc_gbak_page_size_not_allowed)       // 336330760L;
    DEFINE_ECONSTANT(isc_gbak_multi_source_dest)           // 336330761L;
    DEFINE_ECONSTANT(isc_gbak_filename_missing)            // 336330762L;
    DEFINE_ECONSTANT(isc_gbak_dup_inout_names)             // 336330763L;
    DEFINE_ECONSTANT(isc_gbak_inv_page_size)               // 336330764L;
    DEFINE_ECONSTANT(isc_gbak_db_specified)                // 336330765L;
    DEFINE_ECONSTANT(isc_gbak_db_exists)                   // 336330766L;
    DEFINE_ECONSTANT(isc_gbak_unk_device)                  // 336330767L;
    DEFINE_ECONSTANT(isc_gbak_blob_info_failed)            // 336330772L;
    DEFINE_ECONSTANT(isc_gbak_unk_blob_item)               // 336330773L;
    DEFINE_ECONSTANT(isc_gbak_get_seg_failed)              // 336330774L;
    DEFINE_ECONSTANT(isc_gbak_close_blob_failed)           // 336330775L;
    DEFINE_ECONSTANT(isc_gbak_open_blob_failed)            // 336330776L;
    DEFINE_ECONSTANT(isc_gbak_put_blr_gen_id_failed)       // 336330777L;
    DEFINE_ECONSTANT(isc_gbak_unk_type)                    // 336330778L;
    DEFINE_ECONSTANT(isc_gbak_comp_req_failed)             // 336330779L;
    DEFINE_ECONSTANT(isc_gbak_start_req_failed)            // 336330780L;
    DEFINE_ECONSTANT(isc_gbak_rec_failed)                  // 336330781L;
    DEFINE_ECONSTANT(isc_gbak_rel_req_failed)              // 336330782L;
    DEFINE_ECONSTANT(isc_gbak_db_info_failed)              // 336330783L;
    DEFINE_ECONSTANT(isc_gbak_no_db_desc)                  // 336330784L;
    DEFINE_ECONSTANT(isc_gbak_db_create_failed)            // 336330785L;
    DEFINE_ECONSTANT(isc_gbak_decomp_len_error)            // 336330786L;
    DEFINE_ECONSTANT(isc_gbak_tbl_missing)                 // 336330787L;
    DEFINE_ECONSTANT(isc_gbak_blob_col_missing)            // 336330788L;
    DEFINE_ECONSTANT(isc_gbak_create_blob_failed)          // 336330789L;
    DEFINE_ECONSTANT(isc_gbak_put_seg_failed)              // 336330790L;
    DEFINE_ECONSTANT(isc_gbak_rec_len_exp)                 // 336330791L;
    DEFINE_ECONSTANT(isc_gbak_inv_rec_len)                 // 336330792L;
    DEFINE_ECONSTANT(isc_gbak_exp_data_type)               // 336330793L;
    DEFINE_ECONSTANT(isc_gbak_gen_id_failed)               // 336330794L;
    DEFINE_ECONSTANT(isc_gbak_unk_rec_type)                // 336330795L;
    DEFINE_ECONSTANT(isc_gbak_inv_bkup_ver)                // 336330796L;
    DEFINE_ECONSTANT(isc_gbak_missing_bkup_desc)           // 336330797L;
    DEFINE_ECONSTANT(isc_gbak_string_trunc)                // 336330798L;
    DEFINE_ECONSTANT(isc_gbak_cant_rest_record)            // 336330799L;
    DEFINE_ECONSTANT(isc_gbak_send_failed)                 // 336330800L;
    DEFINE_ECONSTANT(isc_gbak_no_tbl_name)                 // 336330801L;
    DEFINE_ECONSTANT(isc_gbak_unexp_eof)                   // 336330802L;
    DEFINE_ECONSTANT(isc_gbak_db_format_too_old)           // 336330803L;
    DEFINE_ECONSTANT(isc_gbak_inv_array_dim)               // 336330804L;
    DEFINE_ECONSTANT(isc_gbak_xdr_len_expected)            // 336330807L;
    DEFINE_ECONSTANT(isc_gbak_open_bkup_error)             // 336330817L;
    DEFINE_ECONSTANT(isc_gbak_open_error)                  // 336330818L;
    DEFINE_ECONSTANT(isc_gbak_missing_block_fac)           // 336330934L;
    DEFINE_ECONSTANT(isc_gbak_inv_block_fac)               // 336330935L;
    DEFINE_ECONSTANT(isc_gbak_block_fac_specified)         // 336330936L;
    DEFINE_ECONSTANT(isc_gbak_missing_username)            // 336330940L;
    DEFINE_ECONSTANT(isc_gbak_missing_password)            // 336330941L;
    DEFINE_ECONSTANT(isc_gbak_missing_skipped_bytes)       // 336330952L;
    DEFINE_ECONSTANT(isc_gbak_inv_skipped_bytes)           // 336330953L;
    DEFINE_ECONSTANT(isc_gbak_err_restore_charset)         // 336330965L;
    DEFINE_ECONSTANT(isc_gbak_err_restore_collation)       // 336330967L;
    DEFINE_ECONSTANT(isc_gbak_read_error)                  // 336330972L;
    DEFINE_ECONSTANT(isc_gbak_write_error)                 // 336330973L;
    DEFINE_ECONSTANT(isc_gbak_db_in_use)                   // 336330985L;
    DEFINE_ECONSTANT(isc_gbak_sysmemex)                    // 336330990L;
    DEFINE_ECONSTANT(isc_gbak_restore_role_failed)         // 336331002L;
    DEFINE_ECONSTANT(isc_gbak_role_op_missing)             // 336331005L;
    DEFINE_ECONSTANT(isc_gbak_page_buffers_missing)        // 336331010L;
    DEFINE_ECONSTANT(isc_gbak_page_buffers_wrong_param)    // 336331011L;
    DEFINE_ECONSTANT(isc_gbak_page_buffers_restore)        // 336331012L;
    DEFINE_ECONSTANT(isc_gbak_inv_size)                    // 336331014L;
    DEFINE_ECONSTANT(isc_gbak_file_outof_sequence)         // 336331015L;
    DEFINE_ECONSTANT(isc_gbak_join_file_missing)           // 336331016L;
    DEFINE_ECONSTANT(isc_gbak_stdin_not_supptd)            // 336331017L;
    DEFINE_ECONSTANT(isc_gbak_stdout_not_supptd)           // 336331018L;
    DEFINE_ECONSTANT(isc_gbak_bkup_corrupt)                // 336331019L;
    DEFINE_ECONSTANT(isc_gbak_unk_db_file_spec)            // 336331020L;
    DEFINE_ECONSTANT(isc_gbak_hdr_write_failed)            // 336331021L;
    DEFINE_ECONSTANT(isc_gbak_disk_space_ex)               // 336331022L;
    DEFINE_ECONSTANT(isc_gbak_size_lt_min)                 // 336331023L;
    DEFINE_ECONSTANT(isc_gbak_svc_name_missing)            // 336331025L;
    DEFINE_ECONSTANT(isc_gbak_not_ownr)                    // 336331026L;
    DEFINE_ECONSTANT(isc_gbak_mode_req)                    // 336331031L;
    DEFINE_ECONSTANT(isc_gbak_just_data)                   // 336331033L;
    DEFINE_ECONSTANT(isc_gbak_data_only)                   // 336331034L;
    DEFINE_ECONSTANT(isc_dsql_too_old_ods)                 // 336397205L;
    DEFINE_ECONSTANT(isc_dsql_table_not_found)             // 336397206L;
    DEFINE_ECONSTANT(isc_dsql_view_not_found)              // 336397207L;
    DEFINE_ECONSTANT(isc_dsql_line_col_error)              // 336397208L;
    DEFINE_ECONSTANT(isc_dsql_unknown_pos)                 // 336397209L;
    DEFINE_ECONSTANT(isc_dsql_no_dup_name)                 // 336397210L;
    DEFINE_ECONSTANT(isc_dsql_too_many_values)             // 336397211L;
    DEFINE_ECONSTANT(isc_dsql_no_array_computed)           // 336397212L;
    DEFINE_ECONSTANT(isc_dsql_implicit_domain_name)        // 336397213L;
    DEFINE_ECONSTANT(isc_dsql_only_can_subscript_array)    // 336397214L;
    DEFINE_ECONSTANT(isc_dsql_max_sort_items)              // 336397215L;
    DEFINE_ECONSTANT(isc_dsql_max_group_items)             // 336397216L;
    DEFINE_ECONSTANT(isc_dsql_conflicting_sort_field)      // 336397217L;
    DEFINE_ECONSTANT(isc_dsql_derived_table_more_columns)  // 336397218L;
    DEFINE_ECONSTANT(isc_dsql_derived_table_less_columns)  // 336397219L;
    DEFINE_ECONSTANT(isc_dsql_derived_field_unnamed)       // 336397220L;
    DEFINE_ECONSTANT(isc_dsql_derived_field_dup_name)      // 336397221L;
    DEFINE_ECONSTANT(isc_dsql_derived_alias_select)        // 336397222L;
    DEFINE_ECONSTANT(isc_dsql_derived_alias_field)         // 336397223L;
    DEFINE_ECONSTANT(isc_dsql_auto_field_bad_pos)          // 336397224L;
    DEFINE_ECONSTANT(isc_dsql_cte_wrong_reference)         // 336397225L;
    DEFINE_ECONSTANT(isc_dsql_cte_cycle)                   // 336397226L;
    DEFINE_ECONSTANT(isc_dsql_cte_outer_join)              // 336397227L;
    DEFINE_ECONSTANT(isc_dsql_cte_mult_references)         // 336397228L;
    DEFINE_ECONSTANT(isc_dsql_cte_not_a_union)             // 336397229L;
    DEFINE_ECONSTANT(isc_dsql_cte_nonrecurs_after_recurs)  // 336397230L;
    DEFINE_ECONSTANT(isc_dsql_cte_wrong_clause)            // 336397231L;
    DEFINE_ECONSTANT(isc_dsql_cte_union_all)               // 336397232L;
    DEFINE_ECONSTANT(isc_dsql_cte_miss_nonrecursive)       // 336397233L;
    DEFINE_ECONSTANT(isc_dsql_cte_nested_with)             // 336397234L;
    DEFINE_ECONSTANT(isc_dsql_col_more_than_once_using)    // 336397235L;
    DEFINE_ECONSTANT(isc_dsql_unsupp_feature_dialect)      // 336397236L;
    DEFINE_ECONSTANT(isc_dsql_cte_not_used)                // 336397237L;
    DEFINE_ECONSTANT(isc_gsec_cant_open_db)                // 336723983L;
    DEFINE_ECONSTANT(isc_gsec_switches_error)              // 336723984L;
    DEFINE_ECONSTANT(isc_gsec_no_op_spec)                  // 336723985L;
    DEFINE_ECONSTANT(isc_gsec_no_usr_name)                 // 336723986L;
    DEFINE_ECONSTANT(isc_gsec_err_add)                     // 336723987L;
    DEFINE_ECONSTANT(isc_gsec_err_modify)                  // 336723988L;
    DEFINE_ECONSTANT(isc_gsec_err_find_mod)                // 336723989L;
    DEFINE_ECONSTANT(isc_gsec_err_rec_not_found)           // 336723990L;
    DEFINE_ECONSTANT(isc_gsec_err_delete)                  // 336723991L;
    DEFINE_ECONSTANT(isc_gsec_err_find_del)                // 336723992L;
    DEFINE_ECONSTANT(isc_gsec_err_find_disp)               // 336723996L;
    DEFINE_ECONSTANT(isc_gsec_inv_param)                   // 336723997L;
    DEFINE_ECONSTANT(isc_gsec_op_specified)                // 336723998L;
    DEFINE_ECONSTANT(isc_gsec_pw_specified)                // 336723999L;
    DEFINE_ECONSTANT(isc_gsec_uid_specified)               // 336724000L;
    DEFINE_ECONSTANT(isc_gsec_gid_specified)               // 336724001L;
    DEFINE_ECONSTANT(isc_gsec_proj_specified)              // 336724002L;
    DEFINE_ECONSTANT(isc_gsec_org_specified)               // 336724003L;
    DEFINE_ECONSTANT(isc_gsec_fname_specified)             // 336724004L;
    DEFINE_ECONSTANT(isc_gsec_mname_specified)             // 336724005L;
    DEFINE_ECONSTANT(isc_gsec_lname_specified)             // 336724006L;
    DEFINE_ECONSTANT(isc_gsec_inv_switch)                  // 336724008L;
    DEFINE_ECONSTANT(isc_gsec_amb_switch)                  // 336724009L;
    DEFINE_ECONSTANT(isc_gsec_no_op_specified)             // 336724010L;
    DEFINE_ECONSTANT(isc_gsec_params_not_allowed)          // 336724011L;
    DEFINE_ECONSTANT(isc_gsec_incompat_switch)             // 336724012L;
    DEFINE_ECONSTANT(isc_gsec_inv_username)                // 336724044L;
    DEFINE_ECONSTANT(isc_gsec_inv_pw_length)               // 336724045L;
    DEFINE_ECONSTANT(isc_gsec_db_specified)                // 336724046L;
    DEFINE_ECONSTANT(isc_gsec_db_admin_specified)          // 336724047L;
    DEFINE_ECONSTANT(isc_gsec_db_admin_pw_specified)       // 336724048L;
    DEFINE_ECONSTANT(isc_gsec_sql_role_specified)          // 336724049L;
    DEFINE_ECONSTANT(isc_license_no_file)                  // 336789504L;
    DEFINE_ECONSTANT(isc_license_op_specified)             // 336789523L;
    DEFINE_ECONSTANT(isc_license_op_missing)               // 336789524L;
    DEFINE_ECONSTANT(isc_license_inv_switch)               // 336789525L;
    DEFINE_ECONSTANT(isc_license_inv_switch_combo)         // 336789526L;
    DEFINE_ECONSTANT(isc_license_inv_op_combo)             // 336789527L;
    DEFINE_ECONSTANT(isc_license_amb_switch)               // 336789528L;
    DEFINE_ECONSTANT(isc_license_inv_parameter)            // 336789529L;
    DEFINE_ECONSTANT(isc_license_param_specified)          // 336789530L;
    DEFINE_ECONSTANT(isc_license_param_req)                // 336789531L;
    DEFINE_ECONSTANT(isc_license_syntx_error)              // 336789532L;
    DEFINE_ECONSTANT(isc_license_dup_id)                   // 336789534L;
    DEFINE_ECONSTANT(isc_license_inv_id_key)               // 336789535L;
    DEFINE_ECONSTANT(isc_license_err_remove)               // 336789536L;
    DEFINE_ECONSTANT(isc_license_err_update)               // 336789537L;
    DEFINE_ECONSTANT(isc_license_err_convert)              // 336789538L;
    DEFINE_ECONSTANT(isc_license_err_unk)                  // 336789539L;
    DEFINE_ECONSTANT(isc_license_svc_err_add)              // 336789540L;
    DEFINE_ECONSTANT(isc_license_svc_err_remove)           // 336789541L;
    DEFINE_ECONSTANT(isc_license_eval_exists)              // 336789563L;
    DEFINE_ECONSTANT(isc_gstat_unknown_switch)             // 336920577L;
    DEFINE_ECONSTANT(isc_gstat_retry)                      // 336920578L;
    DEFINE_ECONSTANT(isc_gstat_wrong_ods)                  // 336920579L;
    DEFINE_ECONSTANT(isc_gstat_unexpected_eof)             // 336920580L;
    DEFINE_ECONSTANT(isc_gstat_open_err)                   // 336920605L;
    DEFINE_ECONSTANT(isc_gstat_read_err)                   // 336920606L;
    DEFINE_ECONSTANT(isc_gstat_sysmemex)                   // 336920607L;
    DEFINE_ECONSTANT(isc_fbsvcmgr_bad_am)                  // 336986113L;
    DEFINE_ECONSTANT(isc_fbsvcmgr_bad_wm)                  // 336986114L;
    DEFINE_ECONSTANT(isc_fbsvcmgr_bad_rs)                  // 336986115L;
    DEFINE_ECONSTANT(isc_fbsvcmgr_info_err)                // 336986116L;
    DEFINE_ECONSTANT(isc_fbsvcmgr_query_err)               // 336986117L;
    DEFINE_ECONSTANT(isc_fbsvcmgr_switch_unknown)          // 336986118L;
    DEFINE_ECONSTANT(isc_err_max)                          // 827;

    DEFINE_ECONSTANT(isc_tpb_version1)
    DEFINE_ECONSTANT(isc_tpb_version3)
    DEFINE_ECONSTANT(isc_tpb_consistency)
    DEFINE_ECONSTANT(isc_tpb_concurrency)
    DEFINE_ECONSTANT(isc_tpb_shared)
    DEFINE_ECONSTANT(isc_tpb_protected)
    DEFINE_ECONSTANT(isc_tpb_exclusive)
    DEFINE_ECONSTANT(isc_tpb_wait)
    DEFINE_ECONSTANT(isc_tpb_nowait)
    DEFINE_ECONSTANT(isc_tpb_read)
    DEFINE_ECONSTANT(isc_tpb_write)
    DEFINE_ECONSTANT(isc_tpb_lock_read)
    DEFINE_ECONSTANT(isc_tpb_lock_write)
    DEFINE_ECONSTANT(isc_tpb_verb_time)
    DEFINE_ECONSTANT(isc_tpb_commit_time)
    DEFINE_ECONSTANT(isc_tpb_ignore_limbo)
    DEFINE_ECONSTANT(isc_tpb_read_committed)
    DEFINE_ECONSTANT(isc_tpb_autocommit)
    DEFINE_ECONSTANT(isc_tpb_rec_version)
    DEFINE_ECONSTANT(isc_tpb_no_rec_version)
    DEFINE_ECONSTANT(isc_tpb_restart_requests)
    DEFINE_ECONSTANT(isc_tpb_no_auto_undo)
    DEFINE_ECONSTANT(isc_tpb_lock_timeout)

    DEFINE_ECONSTANT(SQL_TEXT)
    DEFINE_ECONSTANT(SQL_VARYING)
    DEFINE_ECONSTANT(SQL_SHORT)
    DEFINE_ECONSTANT(SQL_LONG)
    DEFINE_ECONSTANT(SQL_FLOAT)
    DEFINE_ECONSTANT(SQL_DOUBLE)
    DEFINE_ECONSTANT(SQL_D_FLOAT)
    DEFINE_ECONSTANT(SQL_TIMESTAMP)
    DEFINE_ECONSTANT(SQL_BLOB)
    DEFINE_ECONSTANT(SQL_ARRAY)
    DEFINE_ECONSTANT(SQL_QUAD)
    DEFINE_ECONSTANT(SQL_TYPE_TIME)
    DEFINE_ECONSTANT(SQL_TYPE_DATE)
    DEFINE_ECONSTANT(SQL_INT64)
    DEFINE_ECONSTANT(SQL_DATE)
    DEFINE_ECONSTANT(SQL_DIALECT_V5)
    DEFINE_ECONSTANT(SQL_DIALECT_V6_TRANSITION)
    DEFINE_ECONSTANT(SQL_DIALECT_V6)
    DEFINE_ECONSTANT(SQL_DIALECT_CURRENT)
    return 0;
}
//-----------------------------------------------------//
#define TM_YEAR_BASE    1900
#define ALT_E           0x01
#define ALT_O           0x02
#define LEGAL_ALT(x)    { if (alt_format & ~(x)) return (0); }

static const char *day[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday"
};
static const char *abday[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[12] = {
    "January", "February",  "March",   "April",    "May", "June", "July",
    "August",  "September", "October", "November", "December"
};
static const char *abmon[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
    "AM", "PM"
};


static int conv_num(const char **buf, int *dest, int llim, int ulim) {
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;

    if ((**buf < '0') || (**buf > '9'))
        return 0;

    do {
        result *= 10;
        result += *(*buf)++ - '0';
        rulim  /= 10;
    } while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

    if ((result < llim) || (result > ulim))
        return 0;

    *dest = result;
    return 1;
}

char *ci_strptime(const char *buf, const char *fmt, struct tm *tm) {
    char       c;
    const char *bp;
    size_t     len = 0;
    int        alt_format, i, split_year = 0;

    bp = buf;

    while ((c = *fmt) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace(c)) {
            while (isspace(*bp))
                bp++;

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;


again:          switch (c = *fmt++) {
            case '%':           /* "%%" is converted to "%". */
literal:
                if (c != *bp++)
                    return 0;
                break;

            /*
             * "Alternative" modifiers. Just set the appropriate flag
             * and start over again.
             */
            case 'E':           /* "%E?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_E;
                goto again;

            case 'O':           /* "%O?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_O;
                goto again;

            /*
             * "Complex" conversion rules, implemented through recursion.
             */
            case 'c':           /* Date and time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = ci_strptime(bp, "%x %X", tm)))
                    return 0;
                break;

            case 'D':           /* The date as "%m/%d/%y". */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%m/%d/%y", tm)))
                    return 0;
                break;

            case 'R':           /* The time as "%H:%M". */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%H:%M", tm)))
                    return 0;
                break;

            case 'r':           /* The time in 12-hour clock representation. */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%I:%M:%S %p", tm)))
                    return 0;
                break;

            case 'T':           /* The time as "%H:%M:%S". */
                LEGAL_ALT(0);
                if (!(bp = ci_strptime(bp, "%H:%M:%S", tm)))
                    return 0;
                break;

            case 'X':           /* The time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = ci_strptime(bp, "%H:%M:%S", tm)))
                    return 0;
                break;

            case 'x':           /* The date, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = ci_strptime(bp, "%m/%d/%y", tm)))
                    return 0;
                break;

            /*
             * "Elementary" conversion rules.
             */
            case 'A':           /* The day of week, using the locale's form. */
            case 'a':
                LEGAL_ALT(0);
                for (i = 0; i < 7; i++) {
                    /* Full name. */
                    len = strlen(day[i]);
                    if (strncasecmp(day[i], bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abday[i]);
                    if (strncasecmp(abday[i], bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 7)
                    return 0;

                tm->tm_wday = i;
                bp         += len;
                break;

            case 'B':           /* The month, using the locale's form. */
            case 'b':
            case 'h':
                LEGAL_ALT(0);
                for (i = 0; i < 12; i++) {
                    /* Full name. */
                    len = strlen(mon[i]);
                    if (strncasecmp(mon[i], bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abmon[i]);
                    if (strncasecmp(abmon[i], bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 12)
                    return 0;

                tm->tm_mon = i;
                bp        += len;
                break;

            case 'C':           /* The century number. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return 0;

                if (split_year) {
                    tm->tm_year = (tm->tm_year % 100) + (i * 100);
                } else {
                    tm->tm_year = i * 100;
                    split_year  = 1;
                }
                break;

            case 'd':           /* The day of month. */
            case 'e':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
                    return 0;
                break;

            case 'k':           /* The hour (24-hour clock representation). */
                LEGAL_ALT(0);

            /* FALLTHROUGH */
            case 'H':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
                    return 0;
                break;

            case 'l':           /* The hour (12-hour clock representation). */
                LEGAL_ALT(0);

            /* FALLTHROUGH */
            case 'I':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
                    return 0;
                if (tm->tm_hour == 12)
                    tm->tm_hour = 0;
                break;

            case 'j':           /* The day of year. */
                LEGAL_ALT(0);
                if (!(conv_num(&bp, &i, 1, 366)))
                    return 0;
                tm->tm_yday = i - 1;
                break;

            case 'M':           /* The minute. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
                    return 0;
                break;

            case 'm':           /* The month. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &i, 1, 12)))
                    return 0;
                tm->tm_mon = i - 1;
                break;

            case 'p':           /* The locale's equivalent of AM/PM. */
                LEGAL_ALT(0);
                /* AM? */
                if (strcasecmp(am_pm[0], bp) == 0) {
                    if (tm->tm_hour > 11)
                        return 0;

                    bp += strlen(am_pm[0]);
                    break;
                }
                /* PM? */
                else if (strcasecmp(am_pm[1], bp) == 0) {
                    if (tm->tm_hour > 11)
                        return 0;

                    tm->tm_hour += 12;
                    bp          += strlen(am_pm[1]);
                    break;
                }

                /* Nothing matched. */
                return 0;

            case 'S':           /* The seconds. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
                    return 0;
                break;

            case 'U':           /* The week of year, beginning on sunday. */
            case 'W':           /* The week of year, beginning on monday. */
                LEGAL_ALT(ALT_O);

                /*
                 * XXX This is bogus, as we can not assume any valid
                 * information present in the tm structure at this
                 * point to calculate a real value, so just check the
                 * range for now.
                 */
                if (!(conv_num(&bp, &i, 0, 53)))
                    return 0;
                break;

            case 'w':           /* The day of week, beginning on sunday. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
                    return 0;
                break;

            case 'Y':           /* The year. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 9999)))
                    return 0;

                tm->tm_year = i - TM_YEAR_BASE;
                break;

            case 'y':           /* The year within 100 years of the epoch. */
                LEGAL_ALT(ALT_E | ALT_O);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return 0;

                if (split_year) {
                    tm->tm_year = ((tm->tm_year / 100) * 100) + i;
                    break;
                }
                split_year = 1;
                if (i <= 68)
                    tm->tm_year = i + 2000 - TM_YEAR_BASE;
                else
                    tm->tm_year = i + 1900 - TM_YEAR_BASE;
                break;

            /*
             * Miscellaneous conversions.
             */
            case 'n':           /* Any kind of white-space. */
            case 't':
                LEGAL_ALT(0);
                while (isspace(*bp))
                    bp++;
                break;


            default:            /* Unknown/unsupported conversion. */
                return 0;
        }
    }

    /* LINTED functional specification */
    return (char *)bp;
}

CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBConnect, 3)
    T_STRING(0)
    T_STRING(1)
    T_STRING(2)
// db, user, password
    DBHandle * handle = (DBHandle *)malloc(sizeof(DBHandle));

    char  dpb_buffer[256], *dpb;
    short dpb_length = 0;

// isc_attach_database might fail unless we do this
    memset(handle, 0, sizeof(handle));

// Initialise the parameter buffer
    dpb        = dpb_buffer;
    *dpb++     = isc_dpb_version1;
    dpb_length = dpb - dpb_buffer;
    dpb        = dpb_buffer;

// Add user name and password to the DPB.
    isc_modify_dpb(&dpb, &dpb_length, isc_dpb_user_name, PARAM(1), PARAM_LEN(1));
    isc_modify_dpb(&dpb, &dpb_length, isc_dpb_password, PARAM(2), PARAM_LEN(2));

// Attach to the database.
    isc_attach_database(handle->status_vector, PARAM_LEN(0), PARAM(0), &handle->handle, dpb_length, dpb);
    if ((handle->status_vector[0] == 1) && (handle->status_vector[1] > 0)) {
        free(handle);
        //isc_print_status (status_vector);
        RETURN_NUMBER(0)
        return 0;
    }
    RETURN_NUMBER((SYS_INT)handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBDisconnect, 1)
    T_NUMBER(0)
    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);
    if (handle) {
        isc_detach_database(handle->status_vector, &handle->handle);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBErrorCode, 1)
    T_NUMBER(0)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);

    if (!handle) {
        RETURN_NUMBER(-1);
    } else
    if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
        RETURN_NUMBER(/*isc_sqlcode(status_vector)*/ handle->status_vector[1]);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBError, 1)
    T_NUMBER(0)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);
    if (!handle) {
        RETURN_STRING("No connection handle");
    } else
    if ((handle->status_vector[0] == 1) && (handle->status_vector[1] > 0)) {
        char buf[0xFFF];
        buf[0] = 0;
        AnsiString res;
        ISC_STATUS *pvector = (ISC_STATUS *)handle->status_vector;
        while (isc_interprete(buf, &pvector)) {
            if (res.Length())
                res += "\n";
            res += buf;
        }
        RETURN_STRING(res.c_str());
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FBTransaction, 1, 2)
    T_HANDLE(0)

    static char isc_tbp[] = { isc_tpb_version3, isc_tpb_write, isc_tpb_concurrency, isc_tpb_wait };

    char           *p_isc_tbp = isc_tbp;
    char           options[0xFF];
    unsigned short size = (unsigned short)sizeof(isc_tbp);
    if (PARAMETERS_COUNT > 1) {
        T_ARRAY(1)

        float *arr = GetFloatList(PARAMETER(1), Invoke);
        if (arr) {
            p_isc_tbp = options;
            size      = 0;
            for (int i = 0; i < 0xFF; i++) {
                float s = arr[i];
                if (!s)
                    break;
                options[size] = (char)(int)s;
                size++;
            }
            delete[] arr;
        }
    }

    DBHandle *handle = (DBHandle *)(SYS_INT)PARAM(0);

    isc_tr_handle trans = 0L; /* Declare a transaction handle. */
    isc_start_transaction(handle->status_vector, &trans, 1, &handle->handle, size, p_isc_tbp);

    if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
        RETURN_NUMBER(0);
    }
    RETURN_NUMBER((SYS_INT)trans);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FBCommit, 2, 3)
    T_NUMBER(0)
    T_HANDLE(1)

    isc_tr_handle tr_handle = (isc_tr_handle)(SYS_INT)PARAM(0);
    DBHandle *handle = (DBHandle *)(SYS_INT)PARAM(1);

    int retaining = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1);
        retaining = PARAM_INT(1);
    }
    if ((tr_handle) && (handle)) {
        ISC_STATUS res;
        if (retaining)
            res = isc_commit_retaining(handle->status_vector, &tr_handle);
        else
            res = isc_commit_transaction(handle->status_vector, &tr_handle);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FBRollback, 1, 2)
    T_NUMBER(0)
    T_HANDLE(1)

    isc_tr_handle tr_handle = (isc_tr_handle)(SYS_INT)PARAM(0);
    DBHandle *handle = (DBHandle *)(SYS_INT)PARAM(1);

    int retaining = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1);
        retaining = PARAM_INT(1);
    }
    if ((tr_handle) && (handle)) {
        ISC_STATUS res;
        if (retaining)
            res = isc_rollback_retaining(handle->status_vector, &tr_handle);
        else
            res = isc_rollback_transaction(handle->status_vector, &tr_handle);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//------------------------------------------------------------------------
double RoundDouble(double doValue, int nPrecision) {
    static const double doBase = 10.0;
    double doComplete5, doComplete5i;

    doComplete5 = doValue * pow(doBase, (double)(nPrecision + 1));

    if (doValue < 0.0)
        doComplete5 -= 5.0;
    else
        doComplete5 += 5.0;

    doComplete5 /= doBase;
    modf(doComplete5, &doComplete5i);

    return doComplete5i / pow(doBase, (double)nPrecision);
}

XSQLDA *GetParameters(void *arr, INVOKE_CALL _Invoke, isc_stmt_handle stmt, DBHandle *db_handle, isc_tr_handle tr_handle) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    XSQLDA  *in_sqlda = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    if (count > 0) {
        in_sqlda          = (XSQLDA *)malloc(XSQLDA_LENGTH(count));
        in_sqlda->version = SQLDA_VERSION1;
        in_sqlda->sqln    = count;

        isc_dsql_describe_bind(db_handle->status_vector, &stmt, SQLDA_VERSION1, in_sqlda);

        if ((db_handle->status_vector[0] == 1) && db_handle->status_vector[1]) {
            free(in_sqlda);
            return 0;
        }

        int     i = 0;
        XSQLVAR *var;
        for (i = 0, var = in_sqlda->sqlvar; i < in_sqlda->sqld; i++, var++) {
            _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;
                char    is_null = false;

                _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type != VARIABLE_STRING)
                    is_null = true;


                short     dtype = (var->sqltype & ~1);
                int       len   = 0;
                char      *tmp  = 0;
                struct tm timevar;
                switch (dtype) {
                    case SQL_VARYING:
                        var->sqldata = (char *)malloc(sizeof(char) * var->sqllen + 3);
                        len          = (int)nData;
                        if (var->sqllen < len)
                            len = var->sqllen;
                        (*(short *)var->sqldata) = len;
                        memcpy(var->sqldata + 2, szData, len);
                        var->sqldata[len + 2] = 0;
                        break;

                    case SQL_TEXT:
                        var->sqldata    = (char *)malloc(sizeof(char) * var->sqllen + 1);
                        var->sqldata[0] = 0;
                        len             = (int)nData;
                        if (var->sqllen < len)
                            len = var->sqllen;
                        memcpy(var->sqldata, szData, len);
                        var->sqldata[len] = 0;
                        break;

                    case SQL_SHORT:
                        var->sqldata = (char *)malloc(sizeof(short));
                        if (var->sqlscale)
                            *(short *)(var->sqldata) = (short)RoundDouble(atof(szData) / pow(10, var->sqlscale), -var->sqlscale);
                        else
                            *(short *)(var->sqldata) = (short)atoi(szData);
                        break;

                    case SQL_LONG:
                        var->sqldata = (char *)malloc(sizeof(long));
                        if (var->sqlscale)
                            *(long *)(var->sqldata) = (long)RoundDouble(atof(szData) / pow(10, var->sqlscale), -var->sqlscale);
                        else
                            *(long *)(var->sqldata) = atol(szData);
                        break;

                    case SQL_INT64:
                        var->sqldata = (char *)malloc(sizeof(ISC_INT64));
                        if (var->sqlscale)
                            *(ISC_INT64 *)(var->sqldata) = (ISC_INT64)RoundDouble(atof(szData) / pow(10, var->sqlscale), -var->sqlscale);
                        else
                            *(ISC_INT64 *)(var->sqldata) = strtoull(szData, &tmp, 10);
                        break;

                    case SQL_FLOAT:
                        var->sqldata             = (char *)malloc(sizeof(float));
                        *(float *)(var->sqldata) = atof(szData);
                        break;

                    case SQL_DOUBLE:
                        var->sqldata = (char *)malloc(sizeof(double));
                        *(double *)(var->sqldata) = atof(szData);
                        break;

                    case SQL_TYPE_TIME:
                        var->sqldata = (char *)malloc(sizeof(ISC_TIME));
                        ci_strptime(szData, "%H:%M:%S", &timevar);
                        isc_encode_sql_time(&timevar, (ISC_TIME *)var->sqldata);
                        break;

                    case SQL_TYPE_DATE:
                        var->sqldata = (char *)malloc(sizeof(ISC_DATE));
                        ci_strptime(szData, "%Y-%m-%d", &timevar);
                        isc_encode_sql_date(&timevar, (ISC_DATE *)var->sqldata);
                        break;

                    case SQL_TIMESTAMP:
                        var->sqldata = (char *)malloc(sizeof(ISC_TIMESTAMP));
                        ci_strptime(szData, "%Y-%m-%d %H:%M:%S", &timevar);
                        isc_encode_timestamp(&timevar, (ISC_TIMESTAMP *)var->sqldata);
                        break;

                    case SQL_BLOB:
                        {
                            var->sqldata = (char *)malloc(sizeof(ISC_QUAD));
                            memset(var->sqldata, 0, sizeof(ISC_QUAD));
                            isc_blob_handle blob_handle = 0;
                            if (isc_create_blob(db_handle->status_vector, &db_handle->handle, &tr_handle, &blob_handle, /*&blob_id*/ (ISC_QUAD *)var->sqldata)) {
                                break;
                            }
                            len = (int)nData;
                            char *blob_segment = szData;
                            if (len > 0xFFFF) {
                                unsigned short block_size = 0x1FFF;
                                while (len > 0) {
                                    isc_put_segment(db_handle->status_vector, &blob_handle, block_size, blob_segment);
                                    len          -= block_size;
                                    blob_segment += block_size;
                                    if (len < block_size)
                                        block_size = len;
                                }
                            } else
                                isc_put_segment(db_handle->status_vector, &blob_handle, len, blob_segment);
                            isc_close_blob(db_handle->status_vector, &blob_handle);
                            //memcpy(var->sqldata, &blob_id, sizeof(ISC_QUAD));
                        }
                        break;
                }
                if (var->sqltype & 1) {
                    var->sqlind = (short *)malloc(sizeof(short));
                    if (is_null)
                        *(short *)var->sqlind = -1;
                    else
                        *(short *)var->sqlind = 0;
                } else
                    var->sqlind = NULL;
            }
        }
    }
    return in_sqlda;
}

//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBNonQuery, 3)
    T_STRING(0)
    T_HANDLE(1)
    T_NUMBER(2)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(1);
    isc_tr_handle tr_handle = (isc_tr_handle)PARAM_INT(2);

    ISC_STATUS res = isc_dsql_execute_immediate(handle->status_vector, &handle->handle, tr_handle ? &tr_handle : NULL, PARAM_LEN(0), PARAM(0), 3, NULL);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FBQuery, 3, 4)
    T_STRING(0)
    T_HANDLE(1)
    T_NUMBER(2)

    XSQLDA * in_sqlda = NULL;
    DBHandle      *handle   = (DBHandle *)(SYS_INT)PARAM(1);
    isc_tr_handle tr_handle = (isc_tr_handle)PARAM_INT(2);

    isc_stmt_handle stmt    = NULL;
    XSQLDA          *result = (XSQLDA *)malloc(XSQLDA_LENGTH(DEFAULT_NCOLS));
    result->sqld = 12832;
//memset(result, 0, XSQLDA_LENGTH(DEFAULT_NCOLS));
    result->version = SQLDA_VERSION1;
    result->sqln    = DEFAULT_NCOLS;

    isc_dsql_alloc_statement2(handle->status_vector, &handle->handle, &stmt);
    if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
        free(result);
        RETURN_NUMBER(0);
        return 0;
    }
    isc_dsql_prepare(handle->status_vector, &tr_handle, &stmt, PARAM_LEN(0), PARAM(0), 3, NULL);
    if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
        ISC_STATUS status_vector2[20];
        isc_dsql_free_statement(status_vector2, &stmt, DSQL_close);
        free(result);
        RETURN_NUMBER(0);
        return 0;
    }
    if (result->sqld > result->sqln) {
        int n = result->sqld;
        free(result);
        result = (XSQLDA *)malloc(XSQLDA_LENGTH(n));
        //memset(result, 0, XSQLDA_LENGTH(n));
        result->version = SQLDA_VERSION1;
        result->sqln    = n;
        isc_dsql_describe(handle->status_vector, &stmt, 1, result);
        if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
            ISC_STATUS status_vector2[20];
            isc_dsql_free_statement(status_vector2, &stmt, DSQL_close);
            free(result);
            RETURN_NUMBER(0);
            return 0;
        }
    }
    if (PARAMETERS_COUNT > 3) {
        T_ARRAY(3);

        in_sqlda = GetParameters(PARAMETER(3), Invoke, stmt, handle, tr_handle);
    }
    XSQLVAR *var;
    int     i;
    for (i = 0, var = result->sqlvar; i < result->sqld; i++, var++) {
        ISC_SHORT dtype = (var->sqltype & ~1); // drop flag bit for now
        int       size  = -1;
        switch (dtype) {
            case SQL_VARYING:
                var->sqldata = (char *)malloc(sizeof(char) * var->sqllen + 3);
                // ensure null-terminated string
                var->sqldata[sizeof(char) * var->sqllen + 2] = 0;
                break;

            case SQL_TEXT:
                var->sqldata = (char *)malloc(sizeof(char) * var->sqllen + 1);
                // ensure null-terminated string
                var->sqldata[sizeof(char) * var->sqllen] = 0;
                break;

            case SQL_SHORT:
                var->sqldata = (char *)malloc(sizeof(short));
                break;

            case SQL_LONG:
                var->sqldata = (char *)malloc(sizeof(long));
                break;

            case SQL_INT64:
                var->sqldata = (char *)malloc(sizeof(ISC_INT64));
                break;

            case SQL_FLOAT:
                var->sqldata = (char *)malloc(sizeof(float));
                break;

            case SQL_DOUBLE:
                var->sqldata = (char *)malloc(sizeof(double));
                break;

            case SQL_TYPE_TIME:
                var->sqldata = (char *)malloc(sizeof(ISC_TIME));
                break;

            case SQL_TYPE_DATE:
                var->sqldata = (char *)malloc(sizeof(ISC_DATE));
                break;

            case SQL_TIMESTAMP:
                var->sqldata = (char *)malloc(sizeof(ISC_TIMESTAMP));
                break;

            case SQL_BLOB:
                var->sqldata = (char *)malloc(sizeof(ISC_QUAD));
                break;

            default:
                break;
                //printf("oops. unknown type %d\n", dtype);
                //return NULL;
        }
        if (size >= 0) {
            var->sqldata = (char *)malloc(size);
            memset(var->sqldata, 0, size);
        }

        if (var->sqltype & 1) {
            // allocate variable to hold NULL status
            var->sqlind = (short *)malloc(sizeof(short));
        } else
            var->sqlind = NULL;
    }
    if (in_sqlda) {
        isc_dsql_execute2(handle->status_vector, &tr_handle, &stmt, 3, in_sqlda, NULL);

        for (i = 0, var = in_sqlda->sqlvar; i < in_sqlda->sqld; i++, var++) {
            if (var->sqldata)
                free(var->sqldata);
            if (var->sqlind)
                free(var->sqlind);
        }
        free(in_sqlda);
        in_sqlda = 0;
    } else
        isc_dsql_execute(handle->status_vector, &tr_handle, &stmt, 3, NULL);

    if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
        ISC_STATUS status_vector2[20];
        isc_dsql_free_statement(status_vector2, &stmt, DSQL_close);

        for (i = 0, var = result->sqlvar; i < result->sqld; i++, var++) {
            if (var->sqldata)
                free(var->sqldata);
            if (var->sqlind)
                free(var->sqlind);
        }
        free(result);
        RETURN_NUMBER(0);
        return 0;
    }

    QUERY *q = (QUERY *)malloc(sizeof(QUERY));
    q->stmt      = stmt;
    q->row       = result;
    q->tran      = tr_handle;
    q->db_handle = handle;
    RETURN_NUMBER((SYS_INT)q);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBFreeResult, 1)
    T_NUMBER(0)
    QUERY * q = (QUERY *)PARAM_INT(0);
    if (q) {
        SET_NUMBER(0, 0);
        isc_dsql_free_statement(q->db_handle->status_vector, &q->stmt, DSQL_close);

        int     i;
        XSQLVAR *var;
        for (i = 0, var = q->row->sqlvar; i < q->row->sqld; i++, var++) {
            if (var->sqldata)
                free(var->sqldata);
            if (var->sqlind)
                free(var->sqlind);
        }
        free(q->row);
        free(q);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBFetchForward, 1)
    T_HANDLE(0);
    QUERY *q = (QUERY *)PARAM_INT(0);
    if ((q->row) && (q->row->sqld > 0)) {
        ISC_STATUS fetch_stat = isc_dsql_fetch(q->db_handle->status_vector, &q->stmt, 1, q->row);
        if ((q->db_handle->status_vector[0] == 1) && q->db_handle->status_vector[1]) {
            RETURN_NUMBER(0);
            return 0;
        }
        if (fetch_stat == 0) {
            RETURN_NUMBER(1);
            return 0;
        }
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBGetValue, 2)
    T_HANDLE(0);
    T_NUMBER(1);
    QUERY     *q  = (QUERY *)PARAM_INT(0);
    int       col = PARAM_INT(1);
    struct tm timevar;
    char      timestr[256];

    if ((col >= 0) && (col < q->row->sqld)) {
        // is null ?
        if ((q->row->sqlvar[col].sqltype & 1) && (q->row->sqlvar[col].sqlind) && (*(q->row->sqlvar[col].sqlind) != 0)) {
            RETURN_STRING("");
            return 0;
        }
        ISC_SHORT  dtype = (q->row->sqlvar[col].sqltype & ~1); // drop flag bit for now
        AnsiString res;
        switch (dtype) {
            case SQL_BLOB:
                {
                    ISC_QUAD        *blob_id    = (ISC_QUAD *)q->row->sqlvar[col].sqldata;
                    isc_blob_handle blob_handle = NULL;
                    isc_open_blob2(q->db_handle->status_vector, &q->db_handle->handle, &q->tran, &blob_handle, blob_id, 0, NULL);
                    if ((q->db_handle->status_vector[0] == 1) && q->db_handle->status_vector[1]) {
                        RETURN_STRING("");
                        return 0;
                    }

                    unsigned short blob_seg_len = 0xFFFF;
                    char           blob_segment[0xFFFF];
                    while (isc_get_segment(q->db_handle->status_vector, &blob_handle, (unsigned short *)&blob_seg_len, (unsigned short)sizeof(blob_segment), blob_segment) == 0) {
                        res.AddBuffer(blob_segment, blob_seg_len);
                    }

                    isc_close_blob(q->db_handle->status_vector, &blob_handle);
                    if (res.Length()) {
                        RETURN_BUFFER(res.c_str(), res.Length());
                        return 0;
                    } else {
                        RETURN_STRING("");
                        return 0;
                    }
                }
                break;

            case SQL_SHORT:
                if (q->row->sqlvar[col].sqlscale) {
                    res = (double)isc_vax_integer(q->row->sqlvar[col].sqldata, sizeof(SQL_SHORT)) * pow(10, q->row->sqlvar[col].sqlscale);
                } else {
                    res = (long)isc_vax_integer(q->row->sqlvar[col].sqldata, sizeof(SQL_SHORT)); //*(short *)q->row->sqlvar[col].sqldata;
                }
                RETURN_STRING(res.c_str());
                break;

            case SQL_LONG:
                if (q->row->sqlvar[col].sqlscale) {
                    res = (double)isc_vax_integer(q->row->sqlvar[col].sqldata, sizeof(SQL_LONG)) * pow(10, q->row->sqlvar[col].sqlscale);
                } else {
                    res = (long)isc_vax_integer(q->row->sqlvar[col].sqldata, sizeof(SQL_LONG));
                }
                RETURN_STRING(res.c_str());
                break;

            case SQL_INT64:
                {
                    char tmp[40];
                    tmp[0] = 0;
                    if (q->row->sqlvar[col].sqlscale) {
                        double d = ((double)(*(long long *)q->row->sqlvar[col].sqldata)) * pow(10, q->row->sqlvar[col].sqlscale);
                        sprintf(tmp, "%.12g", d);
                    } else {
                        sprintf(tmp, "%lld", *(long long *)q->row->sqlvar[col].sqldata);
                    }
                    RETURN_STRING(tmp);
                }
                break;

            case SQL_FLOAT:
                res = *(float *)q->row->sqlvar[col].sqldata;
                RETURN_STRING(res.c_str());
                break;

            case SQL_DOUBLE:
                res = *(double *)q->row->sqlvar[col].sqldata;
                RETURN_STRING(res.c_str());
                break;

            case SQL_TYPE_TIME:
                isc_decode_sql_time((ISC_TIME *)(q->row->sqlvar[col].sqldata), &timevar);
                strftime(timestr, 255, "%H:%M:%S", &timevar);
                RETURN_STRING(timestr);
                //var->sqldata = (char *)malloc(sizeof(ISC_TIME));
                break;

            case SQL_TYPE_DATE:
                //var->sqldata = (char *)malloc(sizeof(ISC_DATE));
                isc_decode_sql_date((ISC_DATE *)(q->row->sqlvar[col].sqldata), &timevar);
                strftime(timestr, 255, "%Y-%m-%d", &timevar);
                RETURN_STRING(timestr);
                break;

            case SQL_TIMESTAMP:
                //var->sqldata = (char *)malloc(sizeof(ISC_TIMESTAMP));
                isc_decode_timestamp((ISC_TIMESTAMP *)(q->row->sqlvar[col].sqldata), &timevar);
                strftime(timestr, 255, "%Y-%m-%d %H:%M:%S", &timevar);
                RETURN_STRING(timestr);
                break;

            case SQL_ARRAY:
            case SQL_QUAD:
                RETURN_STRING("");
                return 0;

            case SQL_VARYING:
                {
                    //int len=*(short *)(q->row->sqlvar[col].sqldata);
                    //fprintf(stderr, "Len: %i\n", len);
                    //RETURN_BUFFER((q->row->sqlvar[col].sqldata+2), len);
                    RETURN_STRING(q->row->sqlvar[col].sqldata + 2);
                }
                return 0;

            default:
                RETURN_STRING(q->row->sqlvar[col].sqldata);
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBGetBlobId, 2)
    T_HANDLE(0);
    T_NUMBER(1);
    QUERY *q  = (QUERY *)PARAM_INT(0);
    int   col = PARAM_INT(1);

    if ((col >= 0) && (col < q->row->sqld)) {
        // is null ?
        if ((q->row->sqlvar[col].sqltype & 1) && (q->row->sqlvar[col].sqlind) && (*(q->row->sqlvar[col].sqlind) != 0)) {
            RETURN_NUMBER(0);
            return 0;
        }
        ISC_SHORT dtype = (q->row->sqlvar[col].sqltype & ~1); // drop flag bit for now
        if (dtype == SQL_BLOB) {
            ISC_QUAD *blob_id = (ISC_QUAD *)q->row->sqlvar[col].sqldata;
            RETURN_NUMBER((SYS_INT)blob_id);
            return 0;
        }
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBIsNull, 2)
    T_HANDLE(0);
    T_NUMBER(1);
    QUERY *q  = (QUERY *)PARAM_INT(0);
    int   col = PARAM_INT(1);
    int   res = -1;
    if ((col >= 0) && (col < q->row->sqld)) {
        res = (q->row->sqlvar[col].sqltype & 1) && (q->row->sqlvar[col].sqlind) && (*(q->row->sqlvar[col].sqlind) != 0);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBIsBlob, 2)
    T_HANDLE(0);
    T_NUMBER(1);
    QUERY *q  = (QUERY *)PARAM_INT(0);
    int   col = PARAM_INT(1);
    int   res = 0;
    if ((col >= 0) && (col < q->row->sqld)) {
        ISC_SHORT dtype = (q->row->sqlvar[col].sqltype & ~1); // drop flag bit for now
        if (dtype == SQL_BLOB)
            res = 1;
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBDescribeCol, 2)
    T_HANDLE(0);
    T_NUMBER(1);
    QUERY *q  = (QUERY *)PARAM_INT(0);
    int   col = PARAM_INT(1);
    int   res = 0;
    CREATE_ARRAY(RESULT);
    if ((col >= 0) && (col < q->row->sqld)) {
        ISC_SHORT dtype    = (q->row->sqlvar[col].sqltype & ~1); // drop flag bit for now
        int       nullalbe = (q->row->sqlvar[col].sqltype & 1) && (q->row->sqlvar[col].sqlind);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "name", (INTEGER)VARIABLE_STRING, q->row->sqlvar[col].sqlname, (NUMBER)q->row->sqlvar[col].sqlname_length);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "aliasname", (INTEGER)VARIABLE_STRING, q->row->sqlvar[col].aliasname, (NUMBER)q->row->sqlvar[col].aliasname_length);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "relname", (INTEGER)VARIABLE_STRING, q->row->sqlvar[col].relname, (NUMBER)q->row->sqlvar[col].relname_length);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ownname", (INTEGER)VARIABLE_STRING, q->row->sqlvar[col].ownname, (NUMBER)q->row->sqlvar[col].ownname_length);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dtype);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "subtype", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(q->row->sqlvar[col].sqlsubtype));
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nullable", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)nullalbe);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "scale", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)q->row->sqlvar[col].sqlscale);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBCountColumns, 1)
    T_HANDLE(0);
    QUERY *q = (QUERY *)(SYS_INT)PARAM(0);
    RETURN_NUMBER(q->row->sqld);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBBlobOpen, 2)
    T_HANDLE(0)
    T_HANDLE(1)
    QUERY * q = (QUERY *)PARAM_INT(0);
    ISC_QUAD *blob_id = (ISC_QUAD *)(SYS_INT)PARAM(1);

    isc_blob_handle blob_handle = NULL;
    isc_open_blob2(q->db_handle->status_vector, &q->db_handle->handle, &q->tran, &blob_handle, blob_id, 0, NULL);
    if ((q->db_handle->status_vector[0] == 1) && q->db_handle->status_vector[1]) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)blob_handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBBlobRead, 1)
    T_HANDLE(0)
    T_HANDLE(1)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);

    isc_blob_handle blob_handle  = (isc_blob_handle)(SYS_INT)PARAM(1);
    unsigned short  blob_seg_len = 0;
    char            blob_segment[0xFFFF];
    if (isc_get_segment(handle->status_vector, &blob_handle, (unsigned short *)&blob_seg_len, sizeof(blob_segment), blob_segment) == 0) {
        if (blob_seg_len) {
            RETURN_BUFFER(blob_segment, blob_seg_len)
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBBlobClose, 1)
    T_HANDLE(0)
    T_HANDLE(1)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);
    isc_blob_handle blob_handle = (isc_blob_handle)(SYS_INT)PARAM(1);
    if (blob_handle) {
        isc_close_blob(handle->status_vector, &blob_handle);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBBlobSeek, 4)
    T_HANDLE(0)
    T_HANDLE(1)
    T_NUMBER(2)
    T_NUMBER(3)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);
    isc_blob_handle blob_handle = (isc_blob_handle)(SYS_INT)PARAM(1);

    ISC_LONG   out = 0;
    ISC_STATUS res = isc_seek_blob(handle->status_vector, &blob_handle, (short)PARAM_INT(2), PARAM_INT(3), &out);
    SET_NUMBER(3, out);

    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBBlobWrite, 3)
    T_HANDLE(0)
    T_HANDLE(1)
    T_STRING(2)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);
    isc_blob_handle blob_handle = (isc_blob_handle)(SYS_INT)PARAM(1);

    unsigned short blob_seg_len  = 0;
    char           *blob_segment = PARAM(2);
    int            len           = PARAM_LEN(2);
    ISC_STATUS     res           = 0;
    if (len > 0xFFFF) {
        unsigned short block_size = 0x1FFF;
        while (len > 0) {
            res           = isc_put_segment(handle->status_vector, &blob_handle, block_size, blob_segment);
            len          -= block_size;
            blob_segment += block_size;
            if (len < block_size)
                block_size = len;
        }
    } else
        res = isc_put_segment(handle->status_vector, &blob_handle, len, blob_segment);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBBlobCreate, 4)
    T_HANDLE(0)
    T_HANDLE(1)
    T_HANDLE(2)

    DBHandle * handle = (DBHandle *)(SYS_INT)PARAM(0);
    isc_tr_handle tr_handle = (isc_tr_handle)(SYS_INT)PARAM(1);
    ISC_QUAD      *blob_id  = (ISC_QUAD *)PARAM_INT(2);

    isc_blob_handle blob_handle = NULL;
    isc_create_blob(handle->status_vector, &handle->handle, &tr_handle, &blob_handle, blob_id);
    SET_NUMBER(2, (SYS_INT)&blob_id)
    if ((handle->status_vector[0] == 1) && handle->status_vector[1]) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER((SYS_INT)blob_handle);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FBAffectedRows, 1)
    T_HANDLE(0)
    QUERY * q = (QUERY *)PARAM_INT(0);

    int  length, type, res = 0;
    int  del_count   = 0, ins_count = 0, upd_count = 0, sel_count = 0;
    char type_item[] = { isc_info_sql_stmt_type, isc_info_sql_records };
    char res_buffer[88], *pres;

    pres = res_buffer;

    isc_dsql_sql_info(q->db_handle->status_vector, &q->stmt,
                      sizeof(type_item), type_item,
                      sizeof(res_buffer), res_buffer);
    if ((q->db_handle->status_vector[0] == 1) && (q->db_handle->status_vector[1] > 0)) {
        RETURN_NUMBER(-1);
        return 0;
    }

/* check the type of the statement */
    if (*pres == isc_info_sql_stmt_type) {
        pres++;
        length = isc_vax_integer(pres, 2);
        pres  += 2;
        type   = isc_vax_integer(pres, length);
        pres  += length;
    } else {
        RETURN_NUMBER(-2);
        return 0;
    }

    if (type > 4) {
        RETURN_NUMBER(0);
        return 0;               /* not a SELECT, INSERT, UPDATE or DELETE SQL statement */
    }

    if (*pres == isc_info_sql_records) {
        pres++;
        length = isc_vax_integer(pres, 2);         /* normally 29 bytes */
        pres  += 2;

        while (*pres != 1) {
            switch (*pres) {
                case isc_info_req_select_count:
                    pres++;
                    length    = isc_vax_integer(pres, 2);
                    pres     += 2;
                    sel_count = isc_vax_integer(pres, length);
                    pres     += length;
                    break;

                case isc_info_req_insert_count:
                    pres++;
                    length    = isc_vax_integer(pres, 2);
                    pres     += 2;
                    ins_count = isc_vax_integer(pres, length);
                    pres     += length;
                    break;

                case isc_info_req_update_count:
                    pres++;
                    length    = isc_vax_integer(pres, 2);
                    pres     += 2;
                    upd_count = isc_vax_integer(pres, length);
                    pres     += length;
                    break;

                case isc_info_req_delete_count:
                    pres++;
                    length    = isc_vax_integer(pres, 2);
                    pres     += 2;
                    del_count = isc_vax_integer(pres, length);
                    pres     += length;
                    break;

                default:
                    pres++;
                    break;
            }
        }
    } else {
        RETURN_NUMBER(-3);
        return 0;
    }

    switch (type) {
        case isc_info_sql_stmt_select:
            res = sel_count;
            break;

        case isc_info_sql_stmt_delete:
            res = del_count;
            break;

        case isc_info_sql_stmt_update:
            res = upd_count;
            break;

        case isc_info_sql_stmt_insert:
            res = ins_count;
            break;
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FBColumnName, 2, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    QUERY *q  = (QUERY *)PARAM_INT(0);
    int   col = PARAM_INT(1);
    int   res = 0;
    if ((col >= 0) && (col < q->row->sqld)) {
        ISC_SHORT dtype    = (q->row->sqlvar[col].sqltype & ~1); // drop flag bit for now
        int       nullalbe = (q->row->sqlvar[col].sqltype & 1) && (q->row->sqlvar[col].sqlind);
        if (q->row->sqlvar[col].aliasname_length) {
            RETURN_BUFFER(q->row->sqlvar[col].aliasname, q->row->sqlvar[col].aliasname_length);
        } else
        if (q->row->sqlvar[col].sqlname_length) {
            RETURN_BUFFER(q->row->sqlvar[col].sqlname, q->row->sqlvar[col].sqlname_length);
        } else {
            RETURN_STRING("");
        }
        if (PARAMETERS_COUNT > 2)
            SET_NUMBER(2, dtype);
    } else {
        if (PARAMETERS_COUNT > 2)
            SET_NUMBER(2, -1);
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
