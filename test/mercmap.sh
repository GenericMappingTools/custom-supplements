#!/bin/bash
#	$Id$
#
# Test the Mercator map maker

ps=mercmap.ps

gmt mercmap -R-30/10/0/30 -Crelief -P -W6i -S > $ps

