#include "sway/tree/space.h"
#include "sway/tree/workspace.h"
#include "sway/tree/layout.h"

static struct sway_space_view *space_view_create(struct sway_view *sway_view,
		struct sway_space_container *container, float content_scale) {
	struct sway_space_view *view = malloc(sizeof(struct sway_space_view));
	view->view = sway_view;
	view->container = container;
	view->content_scale = content_scale;
	return view;
}

static void space_view_destroy(struct sway_space_view *view) {
	free(view);
}

static struct sway_space_container *space_container_create(struct sway_container *container,
			struct sway_container *focused, struct sway_space *space) {
	struct sway_space_container *space_container = malloc(sizeof(struct sway_space_container));
	if (container->pending.children) {
		space_container->children = create_list();
		space_container->focused_inactive = NULL;
		for (int i = 0; i < container->pending.children->length; ++i) {
			struct sway_container *con = container->pending.children->items[i];
			struct sway_space_container *space_con = space_container_create(con, focused, space);
			if (con == container->current.focused_inactive_child) {
				space_container->focused_inactive = space_con;
			}
			if (con == focused) {
				space->focused = space_con;
			}
			list_add(space_container->children, space_con);
		}
	} else {
		space_container->children = NULL;
	}
	if (container->view) {
		space_container->view = space_view_create(container->view, space_container,
			container->view->content_scale);
	} else {
		space_container->view = NULL;
	}
	space_container->width_fraction = container->width_fraction;
	space_container->height_fraction = container->height_fraction;
	space_container->layout = container->pending.layout;
	space_container->x = container->pending.x;
	space_container->y = container->pending.y;
	space_container->width = container->pending.width;
	space_container->height = container->pending.height;
	return space_container;
}

static void space_container_destroy(struct sway_space_container *container) {
	if (container->children) {
		for (int i = 0; i < container->children->length; ++i) {
			struct sway_space_container *con = container->children->items[i];
			space_container_destroy(con);
		}
		list_free(container->children);
	}
	if (container->view) {
		space_view_destroy(container->view);
	}
	free(container);
}

static struct sway_space *space_create(const char *name) {
	struct sway_space *space = malloc(sizeof(struct sway_space));
	space->name = strdup(name);
	space->tiling = create_list();
	space->floating = create_list();
	list_add(root->spaces, space);
	return space;
}

static void space_destroy(struct sway_space *space) {
	int idx = list_find(root->spaces, space);
	if (idx >= 0) {
		list_del(root->spaces, idx);
	}
	free(space->name);
	for (int i = 0; i < space->tiling->length; ++i) {
		struct sway_space_container *con = space->tiling->items[i];
		space_container_destroy(con);
	}
	list_free(space->tiling);
	for (int i = 0; i < space->floating->length; ++i) {
		struct sway_space_container *con = space->floating->items[i];
		space_container_destroy(con);
	}
	list_free(space->floating);
	free(space);
}

static struct sway_space *find_space(const char *name) {
	for (int i = 0; i < root->spaces->length; ++i) {
		struct sway_space *space = root->spaces->items[i];
		if (strcmp(space->name, name) == 0) {
			return space;
		}
	}
	return NULL;
}

// Save the current workspace configuration into a space with name
void space_save(struct sway_workspace *workspace, const char *name) {
	struct sway_space *space = find_space(name);
	if (space) {
		space_destroy(space);
	}
	space = space_create(name);

	struct sway_seat *seat = input_manager_current_seat();
	struct sway_container *focused = seat_get_focused_container(seat);
	if (focused && focused->pending.workspace != workspace) {
		focused = NULL;
	}

	for (int i = 0; i < workspace->tiling->length; ++i) {
		struct sway_container *container = workspace->tiling->items[i];
		struct sway_space_container *space_container = space_container_create(container, focused, space);
		list_add(space->tiling, space_container);
	}
	for (int i = 0; i < workspace->floating->length; ++i) {
		struct sway_container *container = workspace->floating->items[i];
		struct sway_space_container *space_container = space_container_create(container, focused, space);
		list_add(space->floating, space_container);
		if (container == focused) {
			space->focused = space_container;
		}
	}
}

// Load the space with name into the current workspace.
// If reset is false, add the space data to the workspace. If it is true, close
// any views not belonging to the space
void space_load(struct sway_workspace *workspace, const char *name, bool reset) {
	struct sway_space *space = find_space(name);
	if (!space) {
		return;
	}
	layout_space_restore(space, workspace, reset);
}
