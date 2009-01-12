# Example ftpd module.
namespace eval ::ngBot::module::example {
	variable ns [namespace current]
	# References the base namespace (the parents parent to be precise) ::ngBot.
	variable np [namespace qualifiers [namespace parent]]
	# List of procs we want to rename and replace with our own. The originals
	# can be chain loaded if need be.
	variable renames [list init_ng readlog]

	# Export all proc's.
	namespace export *

	proc init {} {
		variable np
		variable renames

		# Rename procs so we can chain load them later.
		foreach name $renames {
			rename ${np}::$name ${np}::_$name
		}

		# Run this code in the scope of the base namespace ::ngBot.
		# Because this is being run in a different scope, make sure the variables
		# you're using are available. Don't use ${np} or ${ns} for instance.
		namespace inscope $np {
			# All imported procs will still run within their original namespace.
			# Import more or less just adds pointers to the original procs.
			namespace import -force ::ngBot::module::example::*
		}
	}

	proc init_ng {type} {
		variable ns
		variable np
		# Example of how to reference variables in the base from an ftpd module.
		variable ${np}::sitename

		putlog "\[ngBot\] $sitename :: Loaded ftpd module: [namespace tail $ns]."

		# Chain load the original init_ng proc.
		${np}::_init_ng $type
	}

	# deinit will be called by the base namespace ::ngBot automaticly when needed.
	# Removes the need to create a bind for the prerehash EVNT in every plugin.
	proc deinit {} {
		variable ns
		variable np
		variable renames
		# Example of how to reference variables in the base from an ftpd module.
		variable ${np}::sitename

		# Revert the renames we did earlier. Using .rehash wont reload some base
		# eggdrop/tcl procs, only .reset will.
		foreach name $renames {
			if {[llength [info commands "${np}::_$name"]] == 1} {
				catch {rename ${np}::$name {}}
				catch {rename ${np}::_$name ${np}::$name}
			}
		}

		putlog "\[ngBot\] $sitename :: Unloaded ftpd module: [namespace tail $ns]."
	}

	# readlog is run every second by a timer.
	proc readlog {} {
		variable np
		variable ${np}::sitename

		putlog "\[ngBot\] $sitename :: Readlog proc called"

		# Chain load the original readlog proc.
		${np}::_readlog
	}
}