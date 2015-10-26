" Vim syntax file
" Language: Algo
" Maintainer: Thibaud Michaud
" Latest Revision: 21 April 2015

if exists("b:current_syntax")
  finish
endif

" Keywords
set iskeyword+='
syn keyword algoKeyword algorithme procedure fonction parametres
syn keyword algoKeyword algorithm function parameters
syn keyword algoKeyword globaux locaux variables types constantes
syn keyword algoKeyword global local
syn keyword algoKeyword jusqu'a
syn keyword algoKeyword to
syn keyword algoKeyword debut fin faire alors pour tant que si selon autrement
syn keyword algoKeyword begin end do then for while if caseof default
syn keyword algoKeyword sinon retourne vrai faux
syn keyword algoKeyword else return true false
syn keyword algoTypes entier chaine caractere booleen reel
syn keyword algoTypes int string char boolean real
syn keyword algoFunctions ecrire lire allouer liberer
syn keyword algoFunctions write read malloc free
syn region algoComment start="/\*" end="\*/"

syn region algoString start="\"" skip="\\\"" end="\""
syn match algoNumbers "\s\d\+\(\.\d\+\)\?"

let b:current_syntax = "algo"

hi def link algoKeyword Statement
hi def link algoString String
hi def link algoNumbers Number
hi def link algoTypes Type
hi def link algoFunctions Function
hi def link algoComment Comment
