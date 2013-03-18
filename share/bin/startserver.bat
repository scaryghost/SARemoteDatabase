@ECHO OFF
set url=share\etc\saremotedb.sqlite
set port=12345
set pw=server

@ECHO ON
saremotedatabase --data-url %url% --tcp-port %port% --server-pw %pw% %*
