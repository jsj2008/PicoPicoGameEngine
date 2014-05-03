sz=ppfont:size("あ\nい")

g=ppgraph

function start()
  g:print(sz.width..","..sz.height)
end
