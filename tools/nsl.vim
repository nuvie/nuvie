" Vim syntax file
" Language:	nsl (nuvie spell list)
" Maintainer:	Luteijn
" Last Change:	Tue Nov 16 10:06:07 CET 2004

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn match  NslKeyword	"^\~\|^invocation:\|^script:\|^reagents:\|^name:"

syn match NslComment "^#\s.*$\|^#$" contains=NslTodo
syn match NslComment2 "^#\S.*$" contains=NslTodo

syn keyword NslTodo contained	TODO BUG FIXME XXX

" the labels & commands in the script file, anything else is pushed on the stack
syn match  NslLabel "\s\zs:\w*\|^:\w*" 
syn keyword  NslCommand 	_jmp _jmptrue _jmpfalse _end _call 
syn match  NslFunction "\s\zs_\w*\|^_\w*" 


" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_nsl_syntax_inits")
  if version < 508
    let did_NPC_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink NslLabel		Todo
  HiLink NslTodo		Todo
  HiLink NslKeyword		Statement
  HiLink NslCommand		Constant
  HiLink NslFunction		Identifier
  hi NslComment term=NONE ctermfg=DarkGreen guifg=DarkGreen
  hi NslComment2 term=NONE ctermfg=Green guifg=Green
  delcommand HiLink
endif

let b:current_syntax = "nsl"

" vim: ts=8
