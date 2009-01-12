#################################################################################
# ngBot - Example Plug-in                                                       #
#################################################################################
#
# Description:
# - Example plugin.
# - Shows:
#    * The basics of creating a plugin for ngBot.
#    * How to replace and silence the default output.
#
# Installation:
# 1. Copy this file (Example.tcl) and the plugin theme (Example.zpt) into your
#    pzs-ng sitebots 'plugins' folder.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/Example.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

# All plugins should reside in the ::ngBot::plugin:: namespace!
namespace eval ::ngBot::plugin::Example {
	# Short hand for ::ngBot::plugin::Example
	variable ns [namespace current]
	# Short hand for ::ngBot
	variable np [namespace qualifiers [namespace parent]]

	# NOTE: You CAN NOT declare variables from other namespaces directly within
	#       a namespace eval {}. If you want to manipulate them you will need
	#       to use set/array set/etc.. with the variables full path.
	#
	# This is CORRECT:
	#      set ${np}::disable(NEWDIR) 0
	#
	# This is INCORRECT:
	#      variables ${np}::disable
	#      set disable(NEWDIR) 0
	#
	#       This however does NOT apply to any procs within the namespace eval.

	variable events [list "NEWDIR"]

	variable scriptfile [info script]
	variable scriptname ${ns}::log_event

	# Callee: ::ngBot::init_plugins
	# Desc:   Is not run until AFTER all scripts have been sourced. It is safe
	#         to assume by this point that if sourced, all dependencies are
	#         available (though they might not be init'd). Because of this, it
	#         no longer matters what order scripts are sourced in the
	#         eggdrop.conf file.
	#         If you want to use your own custom error use return -code -1
	proc init {} {
		variable ns
		variable np
		variable events
		variable scriptname
		variable scriptfile
		# Only when declaring variables from other namespaces will you need
		# to use their full paths.
		variable ${np}::variables
		variable ${np}::precommand

		# Since we're more or less cloning NEWDIR, just copy its variables
		# verbatim. The variables are used to match the logdata list elements
		# in the callback proc (::ngBot::plugin::Example::log_event) to a name
		# so we can parse them in the themes.
		set variables(EXAMPLE-NEWDIR) $variables(NEWDIR)

		# Load a custom plugin theme.
		set theme_file [file normalize "[pwd]/[file rootname $scriptfile].zpt"]
		if {[file isfile $theme_file]} {
			# Call ::ngBot::loadtheme {file $theme_file} {isplugin true}
			${np}::loadtheme $theme_file true
		}

		# Register the event handler.
		foreach event $events {
			# Don't need to use the ${np}:: prefix as it was declared earleir.
			# There are 2 types of default events:
			#   precommand      Run before an event, silences the default output
			#                   if the precommand callback returns false.
			#   postcommand     Run after an event.
			lappend precommand($event) $scriptname
		}
	}

	# Callee: ::ngBot::deinit
	# Desc:   Called when the bot is cleaning up. No longer requires a bind
	#         evnt prerehash for each plugin. This proc is also NOT REQUIRED
	#         if all you need to do is: namespace delete [namespace current]
	#         or unbind something that is bound to a proc within a child
	#         namespace of ::ngBot. The callee will delete this namespace if
	#         when the proc returns the namespace still exists and also
	#         unbinds everything bound to a proc matching ::ngBot::*.
	proc deinit {} {
		variable ns
		variable np
		variable events
		variable scriptname
		variable ${np}::precommand

		# Remove the script event callback from precommand.
		foreach event $events {
			if {[info exists precommand($event)] && [set pos [lsearch -exact $precommand($event) $scriptname]] !=  -1} {
				set precommand($event) [lreplace $precommand($event) $pos $pos]
			}
		}

		# Remove the namespace.
		namespace delete $ns
	}

	# Callee: ::ngBot::readlog -> ::ngBot::eventhandler
	# Desc:   Registered precommand callback. Will be called on NEWDIR event.
	proc log_event {event section logdata} {
		variable np

		# Set the event for our custom EXAMPLE-NEWDIR theme from Example.zpt.
		set target "EXAMPLE-NEWDIR"

		# Send to all the channels the NEWDIR event is set to announce
		# to and format the output using the EXAMPLE-NEWDIR theme.
		${np}::sndall $event $section [${np}::ng_format $target $section $logdata]

		# Return false to silence the default output.
		return false
	}
}