procedure clear_script(var sc:Tscript);
var i:integer;
begin
for i:=0 to MAX_SCRIPT_COMMANDS-1 do
  begin
  sc[i]:=' ';
  end;
end;

procedure norm_script(var sc:Tscript);
var indx:integer;
begin
for indx:=0 to MAX_SCRIPT_COMMANDS-1 do
  begin
  while (sc[indx][1]=' ')or(sc[indx][1]=#9)or(sc[indx][1]=#13) do sc[indx]:=copy(sc[indx],2,length(sc[indx])-1);
  end;
end;

procedure execute_script(sc:Tscript);
var param:ansistring;
  int,int2:longint;
  b:boolean;
  itm:PPitem;
  s:ansistring;
begin
for i:=0 to MAX_SCRIPT_COMMANDS-1 do
  begin
  if sc[i][1]=' ' then sc[i]:=copy(sc[i],2,length(sc[i])-1);
  param:=copy(sc[i],pos('(',sc[i])+1,pos(')',sc[i])-pos('(',sc[i])-1);
  if copy(sc[i],1,3)='inc' then
    begin
    if get_param(param,1)='money' then
      begin
      val(get_param(param,2),int);
      money:=money+int;
      add_event_msg('CREDITS FOUND: '+to_str(int),MSG_STR_STD_TIME);
      end;
    if get_param(param,1)='repair' then
      begin
      val(get_param(param,2),int);
      repair_kol:=repair_kol+int;
      s:='Repair Kit Founded';
      if int>1 then s:=s+':'+to_str(int);
      add_event_msg(s,MSG_STR_STD_TIME);
      end;
    end else
  if copy(sc[i],1,3)='dec' then
    begin
    if get_param(param,1)='money' then
      begin
      val(get_param(param,2),int);
      money:=money-int;
      if money<0 then money:=0;
      add_event_msg(to_str(int),MSG_STR_STD_TIME);
      end;
    end;
  if copy(sc[i],1,3)='set' then
    begin
    val(get_param(param,2),int);
    if get_param(param,1)='money' then
      begin
      money:=int;
      if money<0 then money:=0;
      add_event_msg(to_str(int),MSG_STR_STD_TIME);
      end;
    if get_param(param,1)='sp' then
      begin
      skill_points:=int;
      if skill_points<0 then skill_points:=0;
      end;
    if get_param(param,1)='repair' then
      begin
      repair_kol:=int;
      if repair_kol<0 then repair_kol:=0;
      end;
    end;
  if pos('additem',sc[i])=1 then
    begin
    val(get_param(param,1),int);
    val(get_param(param,2),int2);
    itm:=new_item(int,int2);
    inventory_add_item(itm);
    val(get_param(param,3),int);
    s:='Item founded: ';
    b:=true;
    case itm^^.type_ of
      ITM_WEAPON:begin
        modify_weapon(itm,random(int));
        s:=s+bullet_type[itm^^.indx]^.iti.name;
        b:=(pllvl>=bullet_type[itm^^.indx]^.iti.droplvl);
        end;
      ITM_FLD_GEN:begin
        modify_weapon(itm,random(int));
        s:=s+fld_gen[itm^^.indx]^.iti.name;
        b:=(pllvl>=fld_gen[itm^^.indx]^.iti.droplvl);
        end;
      end;
    if b then add_event_msg(s,MSG_STR_STD_TIME) else inventory_remove_item(itm);
    end;
  if pos('avail_lvl',sc[i])=1 then
    begin
    val(get_param(param,1),int2);
    b:=true;
    for int:=0 to MAX_LEVELS-1 do
      if (avail_lvl[int]=int2)or(pass_lvl[int]=int2) then b:=false;
    int:=0;
    while(int<MAX_LEVELS)and(avail_lvl[int]<>-1)do inc(int);
    if b and (int<MAX_LEVELS)and(avail_lvl[int]=-1)then avail_lvl[int]:=int2;
    end;
  if pos('pass_lvl',sc[i])=1 then
    begin
    val(get_param(param,1),int2);
    b:=true;
    for int:=0 to MAX_LEVELS-1 do
      if (pass_lvl[int]=int2) then b:=false;
    int:=0;
    while(int<MAX_LEVELS)and(pass_lvl[int]<>-1)do inc(int);
    if b and (int<MAX_LEVELS)and(pass_lvl[int]=-1)then pass_lvl[int]:=int2;
    end;
  if pos('fps',sc[i])=1 then
    begin
    show_fps:=not show_fps;
    end;
  if pos('game_end',sc[i])<>0 then
  if plship<>nil then
    begin
    show_game_end;
    end;
  end;
end;

procedure destroy_container(var cont:Pcontainer);
begin
if (cont<>nil)and(cont^.ef<>nil) then destroy_effect(cont^.ef^);
dispose(cont);
cont:=nil;
end;

procedure create_random_container(x0,y0:double;exp_:integer);
var i:integer;
begin
i:=0;
while(i<MAX_CONTAINERS)and(cont[i]<>nil)do inc(i);
if (i<MAX_CONTAINERS)and(cont[i]=nil)then
  begin
  new(cont[i]);
  with cont[i]^ do
    begin
    ef:=new_effect(round(x0),round(y0),CONTAINER_EFF_INDX,true,1);
    picked:=false;
    x:=x0;y:=y0;
    i:=0;
    sc[i]:='inc(money,'+to_str(10+3*exp_+5*random(exp_))+')';
    inc(i);
    if rnd(ITEM_DROP_CHANCE) then
      begin
      sc[i]:='additem('+to_str(ITM_WEAPON)+','+to_str(-1)+','+to_str(random(exp_))+')';
      inc(i);
      end;
    if rnd(35) then
      begin
      sc[i]:='inc(repair,1)';
      inc(i);
      end;
    for i:=i to MAX_SCRIPT_COMMANDS-1 do sc[i]:='';
    end;
  end;
end;

procedure update_container(var cont:Pcontainer);
begin
if (cont<>nil)and(plship<>nil) then
with plship^ do
  begin
  if (not cont^.picked)and(cont^.ef<>nil) then
  cont^.picked:=rect_cross(round(x),round(y),round(x)+ship_w(plship),round(y)-sign(dir)*ship_h(plship),
    round(cont^.ef^^.x)-eff_w(cont^.ef^)div 2,round(cont^.ef^^.y)+eff_h(cont^.ef^)div 2,round(cont^.ef^^.x)+eff_w(cont^.ef^)div 2,round(cont^.ef^^.y)-eff_h(cont^.ef^)div 2);
  if (cont^.ef<>nil)and(cont^.picked) then
    begin
    destroy_effect(cont^.ef^);
    cont^.ef:=nil;
    execute_script(cont^.sc);
    destroy_container(cont);
    end;
  end;
end;
