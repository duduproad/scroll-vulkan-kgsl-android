#include <string.h>
#include <strings.h>
#include "sway/commands.h"
#include "util.h"

struct cmd_results *cmd_focus_follows_mouse(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "focus_follows_mouse", EXPECTED_EQUAL_TO, 1))) {
		return error;
	} else if(strcmp(argv[0], "no") == 0) {
		config->focus_follows_mouse = FOLLOWS_NO;
	} else if(strcmp(argv[0], "yes") == 0) {
		config->focus_follows_mouse = FOLLOWS_YES;
	} else if(strcmp(argv[0], "always") == 0) {
		config->focus_follows_mouse = FOLLOWS_ALWAYS;
	} else if(strcmp(argv[0], "full") == 0) {
		config->focus_follows_mouse = FOLLOWS_FULL;
	} else {
		return cmd_results_new(CMD_FAILURE,
				"Expected 'focus_follows_mouse no|yes|always|full'");
	}
	return cmd_results_new(CMD_SUCCESS, NULL);
}
