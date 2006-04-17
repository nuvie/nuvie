grep Magic::function_ Magic.cpp |sort | awk -F: '{print "if (!strcmp(" $3 " { return " $3"; }"}' |sed s/_/,\"/ |sed s/\(\)/\"\)\)/
