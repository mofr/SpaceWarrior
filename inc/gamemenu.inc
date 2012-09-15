const MAX_GAMEMENU_BUTTONS=10;
var menuimage,menumask,scr_img_menu:pimage;
  menuquit,menu_loadgame:boolean;
  gamemenu_bt:array[0..MAX_GAMEMENU_BUTTONS-1]of Pbutton;

procedure create_scr_img_gamemenu(dimg:pimage;menu:boolean);
var i:integer;
begin
composeimage(dimg,scr_img,0,0);
if menu then putimage_wmask(dimg,menuimage,menumask,getimagewidth(dimg)div 2-getimagewidth(menuimage)div 2,getimageheight(dimg)div 2-getimageheight(menuimage)div 2,255);
for i:=0 to MAX_GAMEMENU_BUTTONS-1 do if gamemenu_bt[i]<>nil then draw_button(gamemenu_bt[i]);
with cursor do
  putimage_wmask(scr_img_menu,img^,mask^,mx,my,255);
if show_fps then draw_fps(scr_img);
end;

procedure callback1;
begin
menuquit:=true;
end;
procedure callback2;
begin
end;
procedure callback3;
begin
{create_scr_img_gamemenu(scr_img_menu,false);
putimage(0,0,scr_img_menu);
load_game(file_select('saved_games\',1));}
menu_loadgame:=true;
menuquit:=true;
end;
procedure callback4;
begin
show_options;
end;
procedure callback5;
begin
main_end:=true;
end;

procedure game_menu_init;
var i:integer;
begin
menu_loadgame:=false;
loadimagefile(itdetect,'gfx\other\menu.gif',menuimage,1);
loadimagefile(itdetect,'gfx\other\menu.gif',menumask,2);
scr_img_menu:=cloneimage(scr_img);
menuquit:=false;
for i:=0 to MAX_GAMEMENU_BUTTONS-1 do gamemenu_bt[i]:=nil;
i:=0;
gamemenu_bt[i]:=bt_new(BTT_REGULAR,'gfx\other\menu_buttons.gif',false,1,0,scr_img_menu);
bt_setpos(gamemenu_bt[i],getmaxx div 2-gamemenu_bt[i]^.w div 2,getmaxy div 3);
gamemenu_bt[i]^.act_proc:=@callback1;
{inc(i);
gamemenu_bt[i]:=bt_new(BTT_REGULAR,'gfx\other\menu_buttons.gif',false,2,0,scr_img_menu);
bt_setpos(gamemenu_bt[i],getmaxx div 2-gamemenu_bt[i]^.w div 2,gamemenu_bt[i-1]^.y+gamemenu_bt[i-1]^.h);
gamemenu_bt[i]^.act_proc:=@callback2;}
inc(i);
gamemenu_bt[i]:=bt_new(BTT_REGULAR,'gfx\other\menu_buttons.gif',false,3,0,scr_img_menu);
bt_setpos(gamemenu_bt[i],getmaxx div 2-gamemenu_bt[i]^.w div 2,gamemenu_bt[i-1]^.y+gamemenu_bt[i-1]^.h);
gamemenu_bt[i]^.act_proc:=@callback3;
inc(i);
gamemenu_bt[i]:=bt_new(BTT_REGULAR,'gfx\other\menu_buttons.gif',false,4,0,scr_img_menu);
bt_setpos(gamemenu_bt[i],getmaxx div 2-gamemenu_bt[i]^.w div 2,gamemenu_bt[i-1]^.y+gamemenu_bt[i-1]^.h);
gamemenu_bt[i]^.act_proc:=@callback4;
inc(i);
gamemenu_bt[i]:=bt_new(BTT_REGULAR,'gfx\other\menu_buttons.gif',false,5,0,scr_img_menu);
bt_setpos(gamemenu_bt[i],getmaxx div 2-gamemenu_bt[i]^.w div 2,gamemenu_bt[i-1]^.y+gamemenu_bt[i-1]^.h);
gamemenu_bt[i]^.act_proc:=@callback5;
end;

procedure game_menu_quit;
begin
destroyimage(menuimage);
destroyimage(menumask);
destroyimage(scr_img_menu);
end;

procedure game_menu_loop;
var c:char;
begin
update_mouse_input;
if keypressed then
  begin
  c:=readkey;
  case c of
    #27:menuquit:=true;
    end;
  end;
create_scr_img_gamemenu(scr_img_menu,true);
putimage(0,0,scr_img_menu);
if menuquit then
  begin
  game_menu_quit;
  main_loop_proc:=@level_loop;
  if menu_loadgame then loading_game;
  end;
update_mouse_input;
for i:=0 to MAX_GAMEMENU_BUTTONS-1 do if gamemenu_bt[i]<>nil then update_button(gamemenu_bt[i],pmb,mb,mx,my);
end;
