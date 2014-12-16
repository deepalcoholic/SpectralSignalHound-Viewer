#!/bin/bash

markdown2-2.7 -x cuddled-lists,footnotes,smarty-pants,toc,fenced-code-blocks,tables README.md | sed "s/res\//:\/manual.d\//g" > res/manual.html
