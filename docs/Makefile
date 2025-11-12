# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    		=
SPHINXBUILD   		= sphinx-build
SPHINXMULTIVERSION	= sphinx-multiversion
SPHINXPROJ    		= OpenIndus
SOURCEDIR     		= source
BUILDDIR      		= build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile multiversion

# Build documentation for all versions using sphinx-multiversion
multiversion:
	@$(SPHINXMULTIVERSION) $(SOURCEDIR) $(BUILDDIR)/html $(SPHINXOPTS)
	@cp index.html $(BUILDDIR)/html/index.html
	@echo "Build finished. The HTML pages for all versions are in $(BUILDDIR)/html."

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)