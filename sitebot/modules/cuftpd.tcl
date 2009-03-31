################################################################################
#
#                        ngBot - cuftpd FTP Module
#            Project Zipscript - Next Generation (www.pzs-ng.com)
#    (Based on dZSbot by Dark0n3 - http://kotisivu.raketti.net/darkone/)
#
################################################################################

namespace eval ::ngBot::module::cuftpd {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]
	variable renames [list init_binaries]

	namespace export *

	proc init {} {
		variable np
		variable renames

		foreach name $renames {
			rename ${np}::$name ${np}::_$name
		}

		namespace inscope $np {
			namespace import -force ::ngBot::module::cuftpd::*
		}
	}

	proc init_binaries {} {
		variable np
		variable ${np}::bnc

		catch {unset bnc(CURL) bnc(DF) bnc(INCOMPLETE) bnc(PING) bnc(UPTIME)}
	}

	proc deinit {} {
		variable ns
		variable renames

		foreach name $renames {
			if {[llength [info commands "${np}::_$name"]] == 1} {
				catch {rename ${np}::$name {}}
				catch {rename ${np}::_$name ${np}::$name}
			}
		}

		namespace delete $ns
	}
}