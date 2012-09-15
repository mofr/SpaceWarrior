procedure play_intro_movie(path:ansistring);
var
	player:PmediaGIF;
	timg:pimage;
begin
new(player,openmedia(path));
i:=0;
while not player^.endofmedia and not(keypressed) do
	begin
	timg:=player^.grabframe;
	putimage(getmaxx div 2-getimagewidth(timg)div 2,getmaxy div 2-getimageheight(timg)div 2,timg);
//	destroyimage(timg);
//	putimage(0,0,player^.grabframe);
	delay(player^.getframetime);
	end;
dispose(player,closemedia);
end;

procedure play_intro(path:ansistring);
var
	f:text;
	s:ansistring;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: File not found "'+path+'"')else
	begin
	while not eof(f)do
		begin
		readln(f,s);
		play_intro_movie(s);
		end;
	close(f);
	end;
end;
