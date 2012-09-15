function get_exp(lvl:integer):longint;
begin
if lvl<>0 then
get_exp:=100*(lvl*lvl*lvl-1)div lvl else get_exp:=0;
end;

procedure update_player_exp;
begin
if exp>=next_exp then
    begin
    inc(pllvl);
    cur_exp:=next_exp;
    next_exp:=get_exp(pllvl+1);
    inc(skill_points,4);
  end;
end;
