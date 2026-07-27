#include <stdlib.h>

#include "output.h"

void scene_output_pending_resolution(struct wlr_output *output,
		const struct wlr_output_state *state, int *width, int *height) {
	if (state->committed & WLR_OUTPUT_STATE_MODE) {
		switch (state->mode_type) {
		case WLR_OUTPUT_STATE_MODE_FIXED:
			*width = state->mode->width;
			*height = state->mode->height;
			return;
		case WLR_OUTPUT_STATE_MODE_CUSTOM:
			*width = state->custom_mode.width;
			*height = state->custom_mode.height;
			return;
		}
		abort();
	} else {
		*width = output->width;
		*height = output->height;
	}
}

const struct wlr_output_image_description *scene_output_pending_image_description(
		struct wlr_output *output, const struct wlr_output_state *state) {
	if (state->committed & WLR_OUTPUT_STATE_IMAGE_DESCRIPTION) {
		return state->image_description;
	}
	return output->image_description;
}

void scene_output_state_get_buffer_src_box(const struct wlr_output_state *state,
		struct wlr_fbox *out) {
	out->x = state->buffer_src_box.x;
	out->y = state->buffer_src_box.y;
	// If the source box is unset then default to the whole buffer.
	if (state->buffer_src_box.width == 0.0 &&
			state->buffer_src_box.height == 0.0) {
		out->width = (double)state->buffer->width;
		out->height = (double)state->buffer->height;
	} else {
		out->width = state->buffer_src_box.width;
		out->height = state->buffer_src_box.height;
	}
}
