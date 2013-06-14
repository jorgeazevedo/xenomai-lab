TEMPLATE = subdirs

INCLUDEDIR   = $$(HOME)/.xenomailab/include

SUBDIRS = $${INCLUDEDIR}/blockbase.pro

# build must be last:
CONFIG += ordered
SUBDIRS += settings_gui.pro
