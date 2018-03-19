#!/usr/bin/perl -w

use File::Copy;
use File::Basename;
use File::Spec::Functions;

#These are items CPEROUTER does not want to have from HNDROUTER source
my @cpe_disable_items = ( "EZC","RC", "UDHCPD","IPTABLES", "IGD","NVRAMD");

$num_args = $#ARGV + 1;
if ( $num_args != 4 ) {
	print "\n Usage cperouer_init_config.pl <builddir> <wldir> <routerdir> <conf file>";
	exit;
}

my $build_dir = $ARGV[0];
my $wl_dir = $ARGV[1];
my $router_dir = $ARGV[2];
my $config_file =  $ARGV[3];
my $dot_config = catfile($router_dir,'.config');
my $wladjust_pl = catfile($wl_dir,'wlconf_adjust.pl');
my $brcm_lib_dir = catfile($build_dir,'hostTools','PerlLib');

foreach my $n (@cpe_disable_items) {
	system($wladjust_pl,$router_dir,$config_file,$n,'disable');
}
copy($config_file,$dot_config);
