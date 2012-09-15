procedure load_level(lvl_uin:integer);
var f:text;
  s,path:ansistring;
  i,j,enemycount,allycount:integer;
begin
cur_lvl_uin:=lvl_uin;
lvl_enemies:=0;
for i:=0 to MAX_LEVEL_ENEMY_TYPES-1 do lvl_enemy_types[i].type_:=-1;
for i:=0 to MAX_LEVEL_ALLY_TYPES-1 do lvl_ally_types[i].type_:=-1;
for i:=0 to MAX_LEVEL_BACKGROUNDS-1 do
	begin
	lvl_bg[i].dist:=-1;
	destroyimage(lvl_bg[i].img);
	end;
i:=0;
enemycount:=0;
allycount:=0;
path:='levels\'+to_str(lvl_uin);
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln('Error: File not found "',path,'"') else
while not eof(f) do
  begin
  readln(f,s);
  while pos(';',s)<>0 do s:=copy(s,1,pos(';',s)-1);
  if pos('boss',s)<>0 then val(getval(s,#39),lvl_bosstype);
  if pos('asteroid_types',s)<>0 then
    begin
    lvl_kol_ast_types:=kol_param(s);
    s:=copy(s,pos('(',s)+1,pos(')',s)-pos('(',s)-1);
    for i:=0 to lvl_kol_ast_types-1 do
      val(get_param(s,i+1),lvl_ast_types[i]);
    end;
  if pos('addbg',s)<>0 then
  	begin
  	s:=str_between(s,'(',')');
  	val(get_param(s,1),i);
  	with lvl_bg[i] do
  		begin
  		path:=get_param(s,2);
  		val(get_param(s,3),i);
  		val(get_param(s,4),alpha);
  		dist:=i;
  		end;
  	end;
  if pos('asteroid_prob',s)<>0 then val(getval(s,#39),lvl_ast_prob);
  if pos('asteroid_maxcooldown',s)<>0 then val(getval(s,#39),lvl_ast_max_cooldown);
  if pos('asteroid_minvel',s)<>0 then
    begin
    s:=copy(s,pos('(',s)+1,length(s)-pos('(',s));
    s:=copy(s,1,pos(')',s)-1);
    val(get_param(s,1),lvl_ast_minvelx);lvl_ast_minvelx:=lvl_ast_minvelx/100;
    val(get_param(s,2),lvl_ast_minvely);lvl_ast_minvely:=lvl_ast_minvely/100;
    end;
  if pos('asteroid_veloffs',s)<>0 then
    begin
    s:=copy(s,pos('(',s)+1,length(s)-pos('(',s));
    s:=copy(s,1,pos(')',s)-1);
    val(get_param(s,1),lvl_ast_velxoffs);lvl_ast_velxoffs:=lvl_ast_velxoffs/100;
    val(get_param(s,2),lvl_ast_velyoffs);lvl_ast_velyoffs:=lvl_ast_velyoffs/100;
    end;
  if pos('screens',s)<>0 then val(getval(s,#39),lvl_screens);
  if pos('container_prob',s)<>0 then val(getval(s,#39),lvl_cont_prob);
  if pos('enemy',s)<>0 then if enemycount<MAX_LEVEL_ENEMY_TYPES then
  with lvl_enemy_types[enemycount] do
    begin
    val(getval(s,#39),type_);
    val(getval(s,'"'),kol);
    inc(lvl_enemies,kol);
    inc(enemycount);
    end;
  if pos('ally',s)<>0 then if allycount<MAX_LEVEL_ALLY_TYPES then
  with lvl_ally_types[allycount] do
    begin
    val(getval(s,#39),type_);
    val(getval(s,'"'),kol);
    inc(allycount);
    end;
  if pos('finishscript',s)<>0 then
    begin
    while pos('{',s)<>0 do readln(f,s);
    i:=0;
    repeat
      begin
      lvl_finish_script[i]:=s;
      readln(f,s);
      inc(i);
      end until (i>=MAX_SCRIPT_COMMANDS)or(pos('}',lvl_finish_script[i])<>0);
    for i:=i to MAX_SCRIPT_COMMANDS-1 do lvl_finish_script[i]:='';
    norm_script(lvl_finish_script);
    end;
  end;
close(f);
end;

procedure prepare_level;
var i,j:integer;
begin
randomize;
cam_shake_vel:=vec_null;
cam_shake_force:=vec_null;
cam_shake_dvec:=vec_null;
cam_shake:=false;
cursor.visible:=false;
mousecoords(mx,my);
scr_x:=0;scr_y:=0;
for i:=0 to MAX_CONTAINERS-1 do destroy_container(cont[i]);
for i:=0 to MAX_EVENT_MESSAGES-1 do with ev_msg[i] do
  begin
  exist:=false;
  text:='';
  end;
lvl_accomplished:=false;
player_lose:=false;
//
for i:=0 to MAX_LEVEL_BACKGROUNDS-1 do
if lvl_bg[i].dist<>-1 then
	begin
	if loadimagefile(itdetect,lvl_bg[i].path,lvl_bg[i].img,0)=-1 then writeln(logfile,'Error: file"',lvl_bg[i].path,'" not found')
	else
		begin
		setimagetransparencycolor(lvl_bg[i].img,rgbcolorrgb(0,0,0));
		if lvl_bg[i].alpha<255 then imageSSUBcolor(lvl_bg[i].img,lvl_bg[i].img,rgbcolorrgb(255-lvl_bg[i].alpha,255-lvl_bg[i].alpha,255-lvl_bg[i].alpha));
		end;
	end;
//creating enemies
for i:=0 to MAX_LEVEL_ENEMY_TYPES-1 do
with lvl_enemy_types[i] do
  begin
  if type_<>-1 then
  for j:=1 to kol do
    begin
    ship_new(getmaxx div 5+random(getmaxx*3 div 5),getmaxy*2+random(getmaxy*(lvl_screens-3)),type_,-1,true,CONTROL_AUTO,4);
    end;
  end;
for i:=0 to MAX_LEVEL_ALLY_TYPES-1 do
with lvl_ally_types[i] do
  begin
  if type_<>-1 then
  for j:=1 to kol do
    begin
    ship_new(getmaxx div 5+random(getmaxx*3 div 5),getmaxy div 20,type_,1,false,CONTROL_AUTO,4);
    end;
  end;
if lvl_bosstype=-1 then
  begin
  level_finish_check_proc:=@level_finished_wo_boss;
  boss_ship:=nil;
  end
else
  begin
  level_finish_check_proc:=@level_finished_boss;
  boss_ship:=ship_new(getmaxx div 2,(lvl_screens-1)*getmaxy,lvl_bosstype,-1,true,CONTROL_AUTO,5);
  end;
with plship^ do
  begin
  x:=getmaxx div 2-ship_w(plship) div 2;
  y:=getmaxy div 20+ship_h(plship);
  tx:=x;
  ty:=y;
  end;
hp_sp:=0;
hp_sp_bl:=100;
hp_sp_dir:=1;
lvl_exp_collected:=0;
main_loop_proc:=@level_loop;
game_paused:=false;
ast_cooldown:=random(lvl_ast_max_cooldown);
end;

procedure finish_current_level;
var i:integer;
begin
for i:=0 to MAX_LEVELS-1 do if avail_lvl[i]=cur_lvl_uin then avail_lvl[i]:=-1;
execute_script(lvl_finish_script);
for i:=0 to MAX_EFFECTS-1 do destroy_effect(eff[i]);
for i:=0 to MAX_SHIPS-1 do
  if plship<>ship[i] then destroy_ship(ship[i],false);
for i:=0 to MAX_BULLETS-1 do destroy_bullet(bullet[i],false);
for i:=0 to MAX_PARTICLES-1 do destroy_particle(pt[i]);
for i:=0 to MAX_CONTAINERS-1 do destroy_container(cont[i]);
for i:=0 to MAX_ASTEROIDS-1 do destroy_asteroid(ast[i]);
end;

procedure load_level_name(path:ansistring);
var f:text;
  indx:integer;
  s:ansistring;
begin
val(path,indx);
assign(f,'levels\'+path);
reset(f);
repeat readln(f,s) until eof(f)or(pos('name',s)<>0);
lvl_names[indx]:=getval(s,#39);
close(f);
end;
