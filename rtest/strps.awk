BEGIN{doit = 0}
 { if (doit) print $0 }
 $0 == "%%EndSetup" { doit = 1 }
