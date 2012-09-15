var
	controls_quit:boolean;
	scr_img_controls,controls_img:pimage;
	controls_prev_loop:procedure;

procedure controls_init(prevproc:Tprocedure);
begin
loadimagefile(itdetect,'gfx\other\controls.gif',controls_img,0);
setimagetransparencycolor(controls_img,rgbcolorrgb(255,0,0));
controls_prev_loop:=prevproc;
scr_img_controls:=createimageWH(getmaxx+1,getmaxy+1);
controls_quit:=false;
end;

procedure controls_quit_loop;
begin
destroyimage(controls_img);
destroyimage(scr_img_controls);
end;

procedure create_scr_img_controls(var dimg:pimage;dr_cur:boolean);
var
	x1,x2,y0,dy:integer;
begin
composeimage(dimg,scr_img,0,0);
composeimagec(dimg,controls_img,getmaxx div 2-getimagewidth(controls_img)div 2,getmaxy div 2-getimageheight(controls_img)div 2);
x1:=getmaxx div 2-getmaxx div 6;
x2:=getmaxx div 2-getmaxx div 13;
y0:=getmaxy div 2-getmaxy div 6;
dy:=0;
gf_outtext(dimg,x1,y0+dy,font3,0,0,'1-9',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Items using hotkeys',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'CTRL',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Attack random ready weapon',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Up',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Move ship forward',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Down',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Move ship backward',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Left',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Move ship left',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Right',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Move ship right',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Q ',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Repair ship',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'R ',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Set autorepair',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'C ',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Show,hide statistics panel',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'I ',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Show,hide items panel',255);inc(dy,font3^.h+2);
inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'F5',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Quicksave',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'F9',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Quickload',255);inc(dy,font3^.h+2);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'F12',255);gf_outtext(dimg,x2,y0+dy,font3,0,0,'Make Screenshot',255);inc(dy,font3^.h+2);
if dr_cur then
with cursor do
  putimage_wmask(dimg,img^,mask^,mx,my,255);
end;

procedure controls_loop;
var c:char;
begin
update_mouse_input;
if keypressed then
  begin
  c:=readkey;
  case c of
    #27:controls_quit:=true;
    #0:case readkey of
    	#59:controls_quit:=true;
    	end;
    end;
  end;
create_scr_img_controls(scr_img_controls,true);
putimage(0,0,scr_img_controls);
if controls_quit then
  begin
  controls_quit_loop;
  main_loop_proc:=controls_prev_loop;
  end;
end;
