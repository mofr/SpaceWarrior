function load_item(var f:text):PPitem;
var ty,ind,m0,param0,val0:longint;
  s:ansistring;
begin
load_item:=nil;
readln(f,s);
val(get_param(s,1),ty);
val(get_param(s,2),ind);
load_item:=new_item(ty,ind);
readln(f,s);
while pos('[END]',s)=0 do
  begin
  s:=str_between(s,'(',')');
  val(get_param(s,1),m0);
  val(get_param(s,2),param0);
  val(get_param(s,3),val0);
  add_item_modif(load_item,m0,param0,val0/1000);
  readln(f,s);
  end;
end;

procedure save_item(var f:text;itm:PPitem);
var i:integer;
begin
with itm^^ do
  begin
  writeln(f,type_,',',indx);
  for i:=0 to MAX_ITEM_MODIFICATIONS-1 do if modif[i]<>nil then
    with modif[i]^ do writeln(f,'(',m,',',param,',',round(val*1000),')');
  end;
end;

procedure save_ship_type(var f:text;sht:Pship_type;indx:integer);
var i:integer;
begin
writeln(f,'indx',#39,indx,#39);
with sht^.sk do
begin
writeln(f,'skills');
writeln(f,'{');
writeln(f,'bullet_speed',#39,round(bullet_speed*100),#39);
writeln(f,'move_speed',#39,round(move_speed*100),#39);
writeln(f,'bullet_dmg',#39,round(bullet_dmg*100),#39);
writeln(f,'merchant',#39,round(attack_speed*100),#39);
writeln(f,'}');
end;
with sht^ do
  begin
//  for i:=0 to MAX_SHIP_SLOTS-1 do with sht^.slot0[i] do
//    if exist then
//    writeln(f,'addslot',#39,i,#39,':',itm_type,':','"',dx,'"','*',dy,'*','^',itm_indx,'^','|',hotkey,'|');
  if turnab then i:=1 else i:=0;
  writeln(f,'turnability',#39,i,#39);
  writeln(f,'turnkoef',#39,round(turnkoef*100),#39);
  writeln(f,'eff',#39,eff_indx,#39);
  writeln(f,'scoreprize',#39,score_prize,#39);
  writeln(f,'att_cooldown',#39,att_cooldown,#39);
  writeln(f,'ptype',#39,ptype,#39);
  writeln(f,'life',#39,life,#39);
  writeln(f,'pilot',#39,pilot_indx,#39);
  writeln(f,'mainimg',#39,mainimgpath,#39);
  writeln(f,'imgmask',#39,imgmaskpath,#39);
  end;
end;

procedure save_pilot(var f:text;plt:Ppilot);
begin
with plt^.sk do
begin
writeln(f,'indx',#39,0,#39);
writeln(f,'skills');
writeln(f,'{');
writeln(f,'bullet_speed',#39,round(bullet_speed*100),#39);
writeln(f,'move_speed',#39,round(move_speed*100),#39);
writeln(f,'bullet_dmg',#39,round(bullet_dmg*100),#39);
writeln(f,'merchant',#39,round(merchant*100),#39);
writeln(f,'}');
end;
writeln(f,'name',#39,plt^.name,#39);
end;

procedure save_game(path:ansistring);
var f:text;
  i,t:integer;
label end_;
begin
assign(f,path);
{$i-}rewrite(f);{$i+}
if ioresult<>0 then begin writeln('File error "'+path+'"');goto end_;end;
gf_outtext(nil,getmaxx div 2,getmaxy div 2,font1,1,1,'Saving...',255);
writeln(f,'[PLAYER PILOT]');
save_pilot(f,plship^.plt);
writeln(f,'[END]');
writeln(f,'[PLAYER]');
writeln(f,'money',#39,money,#39);
writeln(f,'exp',#39,exp,#39);
writeln(f,'next_exp',#39,next_exp,#39);
writeln(f,'cur_exp',#39,cur_exp,#39);
writeln(f,'pllvl',#39,pllvl,#39);
writeln(f,'skill_points',#39,skill_points,#39);
writeln(f,'repair_kol',#39,repair_kol,#39);
writeln(f,'[END]');
writeln(f,'[PLAYER SHIP TYPE]');
save_ship_type(f,ship_type[plship^.type_],plship^.type_);
for i:=0 to MAX_SHIP_SLOTS-1 do with plship^.slot[i] do
if exist then
  begin
  if itm<>nil then t:=itm^^.indx else t:=-1;
  writeln(f,'addslot',#39,i,#39,':',itm_type,':','"',dx,'"','*',dy,'*','^',t,'^','|',hotkey,'|');
  end;
writeln(f,'[END]');
writeln(f,'[INVENTORY]');
for i:=0 to MAX_SHIP_SLOTS-1 do if plship^.slot[i].itm<>nil then
  begin
  writeln(f,'invitm(',i,')');
  save_item(f,plship^.slot[i].itm);
  writeln(f,'[END]');
  end;
writeln(f,'[END]');
writeln(f,'[STOREHOUSE]');
for i:=0 to MAX_INVENTORY_ITEMS-1 do
  if (inv[i]<>nil)and(inv[i]^<>nil) then
    begin
    writeln(f,'[ITEM]');
    save_item(f,inv[i]);
    writeln(f,'[END]');
    end;
writeln(f,'[END]');
writeln(f,'[WAREHOUSE]');
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do
  if (ware_itm[i]<>nil)and(ware_itm[i]^<>nil) then
  begin
  writeln(f,'[ITEM]');
  save_item(f,ware_itm[i]);
  writeln(f,'[END]');
  end;
writeln(f,'[END]');
writeln(f,'[AVAILABLE LEVELS]');
for i:=0 to MAX_LEVELS-1 do if avail_lvl[i]<>-1 then writeln(f,avail_lvl[i]);
writeln(f,'[END]');
writeln(f,'[FINISHED LEVELS]');
for i:=0 to MAX_LEVELS-1 do if pass_lvl[i]<>-1 then writeln(f,pass_lvl[i]);
writeln(f,'[END]');
close(f);
end_:
end;

procedure load_game(path:ansistring);
var f:text;
  s:ansistring;
  i:longint;
label end_;
procedure load_player_info;
begin
while pos('[END]',s)=0 do
  begin
  if pos('money',s)<>0 then val(getval(s,#39),money);
  if pos('exp',s)<>0 then val(getval(s,#39),exp);
  if pos('next_exp',s)<>0 then val(getval(s,#39),next_exp);
  if pos('cur_exp',s)<>0 then val(getval(s,#39),cur_exp);
  if pos('pllvl',s)<>0 then val(getval(s,#39),pllvl);
  if pos('repair_kol',s)<>0 then val(getval(s,#39),repair_kol);
  if pos('skill_points',s)<>0 then val(getval(s,#39),skill_points);
  readln(f,s);
  end;
end;
procedure load_avail_levels;
var i:integer;
begin
i:=0;
readln(f,s);
while (pos('[END]',s)=0)and(i<MAX_LEVELS) do
  begin
  val(s,avail_lvl[i]);
  inc(i);
  readln(f,s);
  end;
end;
procedure load_pass_levels;
var i:integer;
begin
i:=0;
readln(f,s);
while (pos('[END]',s)=0)and(i<MAX_LEVELS) do
  begin
  val(s,pass_lvl[i]);
  inc(i);
  readln(f,s);
  end;
end;
procedure load_warehouse;
begin
readln(f,s);
while (pos('[END]',s)=0) do
  begin
  warehouse_add_item(load_item(f));
  readln(f,s);
  end;
end;
procedure load_storehouse;
begin
readln(f,s);
while (pos('[END]',s)=0) do
  begin
  inventory_add_item(load_item(f));
  readln(f,s);
  end;
end;
procedure load_ship_ammunition;
var i,dx,dy:integer;
  hotkey:char;
begin
for i:=0 to MAX_SHIP_SLOTS-1 do
  if plship^.slot[i].itm<>nil then
  begin
  destroy_item(plship^.slot[i].itm^);
  plship^.slot[i].itm:=nil;
  end;
readln(f,s);
while (pos('[END]',s)=0) do
  begin
  if pos('invitm',s)<>0 then s:=str_between(s,'(',')');
  val(s,i);
  plship^.slot[i].itm:=load_item(f);
  readln(f,s);
  end;
end;

begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then begin writeln('File error "'+path+'"');goto end_;end;
gf_outtext(nil,getmaxx div 2,getmaxy div 2,font1,1,1,'Loading...',255);
if main_loop_proc=@onstation_loop then onstation_quit(false);
clear_all;
//LOADING
while not eof(f)do
  begin
  readln(f,s);
  if pos('[PLAYER]',s)<>0 then load_player_info;
  if pos('[PLAYER PILOT]',s)<>0 then
    begin
    destroy_pilot(pilot[0]);
    load_pilot_type(f);
    end;
  if pos('[PLAYER SHIP TYPE]',s)<>0 then
    begin
    destroy_ship_type(ship_type[0]);
    load_ship_type(f);
    plship:=ship_new(getmaxx div 2,getmaxy div 20,0,1,false,CONTROL_MAN,3)^;
    plpilot:=plship^.plt;
    end;
  if pos('[AVAILABLE LEVELS]',s)<>0 then load_avail_levels;
  if pos('[FINISHED LEVELS]',s)<>0 then load_pass_levels;
  if pos('[WAREHOUSE]',s)<>0 then load_warehouse;
  if pos('[STOREHOUSE]',s)<>0 then load_storehouse;
  if pos('[INVENTORY]',s)<>0 then load_ship_ammunition;
  end;
player_auto_repair:=false;
//
close(f);
onstation_init(false,false);
main_loop_proc:=@onstation_loop;
end_:
end;

procedure saving_game;
var s:ansistring;
begin
s:=file_select('saved_games\',2);
save_game('saved_games\'+s);
end;

procedure loading_game;
var s:ansistring;
begin
s:=file_select('saved_games\',1);
load_game('saved_games\'+s);
end;

