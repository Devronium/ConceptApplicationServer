#!/bin/sh

. /etc/rc.subr

name="concept"
rcvar=`set_rcvar`
command="/usr/local/sbin/ConceptServer.con &"
start_cmd=$command
stop_cmd=":"

load_rc_config $name
eval "${rcvar}=\${${rcvar}:-'NO'}"

run_rc_command "$1"