#
# Regular cron jobs for the jsonnet package
#
0 4	* * *	root	[ -x /usr/bin/jsonnet_maintenance ] && /usr/bin/jsonnet_maintenance
