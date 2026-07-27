#ifndef _SWAY_SPACE_H
#define _SWAY_SPACE_H

#include "sway/tree/view.h"

struct sway_space_view {
    struct sway_view *view;
    struct sway_space_container *container;
	float content_scale;
};

struct sway_space_container {
    list_t *children; // struct sway_space_container
    struct sway_space_view *view;

    struct sway_space_container *focused_inactive; // focused inactive child
	// Fraction of the viewport size this container occupies
	double width_fraction;
	double height_fraction;
	enum sway_container_layout layout;
	double x, y;
	double width, height;
};

struct sway_space {
    char *name;
    list_t *tiling; // struct sway_space_container *
    list_t *floating;
    struct sway_space_container *focused;
};

// Save the current workspace configuration into a space with name
void space_save(struct sway_workspace *workspace, const char *name);

// Load the space with name into the current workspace.
// If reset is false, add the space data to the workspace. If it is true, close
// any views not belonging to the space
void space_load(struct sway_workspace *workspace, const char *name, bool reset);


#endif // _SWAY_SPACE_H
