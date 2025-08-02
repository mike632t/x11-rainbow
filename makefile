#
# makefile
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Note - Shell commands must start with a tab character at the beginning
# of each line NOT spaces..!
#
#  30 Jul 23   0.1   - Initial version - MT
#   4 Aug 23         - Added backup files to tar archive - MT
#  11 Nov 24   0.2   - Project name derived from current folder - MT
#   1 Aug 25   0.3   - Include branch in backup archive name - MT
#

PROJECT	= $(strip $(notdir $(abspath $(CURDIR)/.)))

SOURCE	=  $(wildcard *.c) 	# Compile all source files
INCLUDE	=  $(wildcard *.h) 	# Automatically get all include files
BACKUP	=  $(wildcard *.[0-9])
OBJECT	=  $(SOURCE:.c=.o)
PROGRAM	=  $(SOURCE:.c=)

FILES	=  $(SOURCE) $(BACKUP) $(INCLUDE) 
FILES	+= LICENSE README.md makefile make.com x11-lib.opt *.txt *.lis
FILES	+= .gitignore # .gitattributes
LANG	=  LANG_$(shell (echo $$LANG | cut -f 1 -d '_'))

UNAME	=  $(shell uname)
BRANCH	=  $(shell git rev-parse --abbrev-ref HEAD > /dev/null 2>&1 && echo `git rev-parse --abbrev-ref HEAD 2>/dev/null`- || true)

LIBS	=  -lX11 -lXext
FLAGS	=  -fcommon -Wall -pedantic -std=gnu99
FLAGS	+= -Wno-comment -Wno-unused-function #-Wno-deprecated-declarations -Wno-builtin-macro-redefined
FLAGS	+= -D $(LANG)

# Operating system specific settings
ifeq ($(UNAME), NetBSD) 	# NetBSD..
LIBS	+=  -lcompat
FLAGS	+=  -I /usr/X11R7/include/ -L /usr/X11R7/lib/ -R /usr/X11R7/lib
endif

# Compiler specific settings
ifeq ($(CC), cc)
#LIBS	+=  
FLAGS	+=  -no-pie
endif

ifdef DEBUG
FLAGS	+=  -DDEBUG
FLAGS	+=  -g
endif

make:$(PROGRAM) $(OBJECT)

all:clean $(PROGRAM) $(OBJECT)

# Compile sources
%.o : %.c
ifdef DEBUG
	@echo $(CC) $(FLAGS) -c $<
endif
	@$(CC) $(FLAGS) -c $<

# Link object file and display executable file to indicate progress
# and validate that it was created
%: %.o
ifdef DEBUG
	@echo $(CC) $(FLAGS) -o $@ $<  $(LIBS)
endif
	@$(CC) $(FLAGS) -o $@ $<  $(LIBS)
	@ls --color $@

clean:
	@rm -f $(OBJECT) # -v
	@rm -f $(PROGRAM) # -v

backup: clean
	@echo "$(PROJECT)-$(BRANCH)`date +'%Y%m%d%H%M'`.tar.gz"; tar -czpf ..\/$(PROJECT)-$(BRANCH)`date +'%Y%m%d%H%M'`.tar.gz $(FILES)
