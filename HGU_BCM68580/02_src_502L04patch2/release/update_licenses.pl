#!/usr/bin/perl

# <:copyright-BRCM:2011:proprietary:standard
#
#    Copyright (c) 2011 Broadcom 
#    All Rights Reserved
#
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
#
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
#
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>

use strict;
use warnings;
use Getopt::Long;
use File::Find;

my $stage;
my $backup;
my $checkonly;
my $fix;
my $verbose;
my @slaclass;
GetOptions(
    'stage=i',    \$stage,    'backup',  \$backup,  'checkonly', \$checkonly,
    'slaclass=s', \@slaclass, 'verbose', \$verbose, 'fix',       \$fix,
);
@slaclass = split( /[, ]+/, join( ',', @slaclass ) );

die("specify either --stage=1 or --stage=2") unless $stage;
my $oss_check = $checkonly && ( $stage == 2 );

my @errors;

if (!$ARGV[0]) {
  die("need at least one arg");
}
if ($ARGV[0] ne '-')
{
  find( \&wanted, @ARGV );
} else {
   while (my $f = <>) 
   {
      chomp($f);
      if (-f "$f") {
          find( \&wanted, $f );
      }
   }
}

exit( scalar(@errors) );

sub wanted {
    my $p = $_;
    return unless -f $p;
    return if $p =~ /\.(gz|tgz|zip|bz2|bin)$/;
    my @stats = stat($p);
    if ( $stats[7] > 30000000 ) {
        print "$File::Find::name is huge -- skipping\n";
        return;
    }

    #print "$p\n";
    open( F, "<", $p );
    my $fbefore   = '';
    my $lblock    = '';
    my $newlblock = '';
    my $fafter    = '';
    my $i         = 0;
    my $changed   = 0;
    my $ltype;
    my $slatype;
    my $iscopyright;
    my $years;
    my $prefix;
    my $l;
    my $is_bcm   = 0;
    my $is_gplok = 0;

    while ( $l = <F> ) {
        return if ( length($l) > 2000 );
        if ( !$fbefore ) {

            # First read
            my @start = unpack( "CCCCCC", $l );
            if ( $start[0] == 0x7f ) {
                # print "file $File::Find::name starts with 0x7f -- skipping\n";
                return;
            }
        }

        last if $l =~ /<:(copyright|label)-(BRCM|3PIP):.*:/;
        if ( $i++ > 500 ) {
            $l = '';
            last;
        }
        $fbefore .= $l;
        if ( $l =~ /copyright.*broadcom/i ) {
            $is_bcm = 1;
        }
        if ( $l =~ /GNU\s+(Lesser\s+)?General/i ) {
            $is_gplok = 1;
        }
        elsif ( $l =~ /Licensed under (the )?\S*GPL/i ) {
            $is_gplok = 1;
        }
        elsif ( $l =~ /Permission is hereby granted for the distribution of this firmware/i ) {
            $is_gplok = 1;
        }
        elsif ( $l =~ /Permission to use, copy, modify, and.or distribute this software for any/i ) {
            $is_gplok = 1;
        }
        elsif ( $l =~ /This program is free software/i ) {
            $is_gplok = 1;
        }
    }

    if ($l) {

        if ( $l =~ /^(.*)<:(copyright|label)-(?:BRCM|3PIP):([\w\s,-]*):([\w\/-]+):(\w*)\s*$/ ) {

            #print "prefix: $1\n";
            #print "years: $3\n";
            #print "licnse: $4\n";
            #print "class: $5\n";
            $years       = $3;
            $ltype       = $4;
            $prefix      = $1;
            $iscopyright = $2;
            $slatype     = $5;
            if ( $years =~ /^\s*$/ ) {
                my @t = localtime(time);
                $years = $t[5] + 1900;
                $changed++;
                $l =~ s/<:(copyright|label)-(BRCM|3PIP):\s*:/<:$1-$2:$years:/;
            }
            $fbefore .= $l;
        }
        else {
            die("failed parse on '$l' in $File::Find::name");
        }

        # slurp license block
        $i = 0;
        while ( $l = <F> ) {

            #print "lblock line read '$l'\n";
            die("very long line?") if ( length($l) > 2000 );
            last if $l =~ /:>/m;
            $lblock .= $l;
            return if $i++ > 500;
        }
        $fafter = $l;

        # regenerate license block

        my @lts = split( /\//, $ltype );
        if ( $stage == 2 ) {
            if ( @lts < 2 ) {
                if ($oss_check) {
                    my $e = "ERROR: wrong license $ltype for $File::Find::name";
                    print "$e\n";
                    push @errors, $e;
                }
                return;
            }
            $ltype = $lts[1];
        }
        else {
            $ltype = $lts[0];
            if ( @slaclass && !grep( ( $_ eq $slatype ), @slaclass ) ) {
                my $e = "ERROR: wrong class $slatype for  $File::Find::name";
                push @errors, $e;
                print "$e\n";
            }
        }

        my $labeltype_cr = ( $iscopyright eq 'copyright' );

        if ($fix) {
            if ( !$is_bcm && !$labeltype_cr && ($ltype ne "NONE") && ($fbefore !~ /copyright/i) && ( $fbefore =~ s/label-BRCM/copyright-BRCM/ ) ) {
                $labeltype_cr = 1;
                $changed      = 1;
            }
        }
        $newlblock = license( $labeltype_cr, $ltype, $years, $prefix );

        # if unchanged, we're donw

        return if ( ( $changed == 0 ) && ( $newlblock eq $lblock ) );

        if ( $changed == 0 ) {
            my $b1 = $lblock;
            my $b2 = $newlblock;
            $b1 =~ s/\s+/ /sg;
            $b2 =~ s/\s+/ /sg;
            if ( $b1 eq $b2 ) {
                return;
            }
        }

        # otherwise, move file to backup and write new, then set perms the same

        if ($checkonly && ($stage == 1)) {
            print "ERROR: License tag is wrong, corrupted, or missing copyright in $File::Find::name \n" if $verbose;
	    push @errors, "tag is bad $p";
            return;
        }
        open( FO, ">", "$p.new" );
        print FO "$fbefore$newlblock$fafter";
        while ( $l = <F> ) {
            print FO $l;
        }
        close(F);
        close(FO);
        chmod( ( $stats[2] & 07777 ) | 0600, "$p.new" );
        if ($backup) {
            rename( $p, "$p.bak" );
        }
        else {
            unlink($p);
        }
        rename( "$p.new", $p );
    }
    else {

        # no tag found ... enforce checks
        return if ( $stage != 2 );
        return
          if ( ( $p eq 'Makefile' )
            || ( $p =~ /\.mak$/ )
            || ( $p =~ /\.html$/ ) );
        if ( $oss_check && $is_bcm && !$is_gplok ) {
            print "WARNING: missing GPL $File::Find::name\n";
        }
    }
}

sub license {
    my $iscopyright = shift;
    my $ltype       = shift;
    my $years       = shift;
    my $prefix      = shift;
    my $l;
    $l = $iscopyright
      ? qq{
   Copyright (c) %YEARS% Broadcom 
   All Rights Reserved
}
      : '';
    if ( $ltype eq 'proprietary' ) {
        $l .= q{
 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
};
    }
    elsif ( $ltype eq 'DUAL' ) {
        $l .= q{
Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

}

    }
    elsif ( $ltype eq 'GPL' ) {
        $l .= q{
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

}
    }
    elsif ( $ltype eq 'NONE' ) {

        # no license insertion
        $l .= "\n";
    }
    else {
        die("license error -- $ltype");
    }

    $l =~ s/%YEARS%/$years/;
    $l =~ s/^/$prefix/mg;

    return ($l);
}

