procedure show_game_end;
var img:pimage;
	time,t:double;
begin
img:=createimageWH(getmaxx+1,getmaxy+1);
time:=0;
t:=15;
while time<t do
	begin
	getimage(0,0,img);
	imageSSUBcolor(img,img,rgbcolorrgb(round(255*time/t),round(255*time/t),round(255*time/t)));
	putimage(0,0,img);
	time:=time+getclocks-prevclocks;
	end;
destroyimage(img);
loadimagefile(itdetect,'gfx\other\end_scene.jpg',img,0);
putimage(0,0,img);
repeat until keypressed;
destroyimage(img);
end;
