# ----------------------------
# Makefile Options
# ----------------------------

NAME = BLOKDUDE
ICON = icon.png
DESCRIPTION = "Blockdude CE"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
