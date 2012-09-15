procedure update_camera_shaking(time:double);
begin
if (vec_len(cam_shake_vel)<2)and(vec_len(cam_shake_dvec)<2) then
	begin
  cam_shake:=false;
  cam_shake_force:=vec_null;
  cam_shake_vel:=vec_null;
  cam_shake_dvec:=vec_null;
  end;
cam_shake_force:=vec_neg(vec_mul(cam_shake_dvec,vec_len(cam_shake_dvec)));
cam_shake_vel:=vec_sum(cam_shake_vel,vec_mul(cam_shake_force,time*2));
cam_shake_vel:=vec_mul(cam_shake_vel,0.935);
cam_shake_vel:=vec_setangrel(cam_shake_vel,random(10)*pi/180);
cam_shake_dvec:=vec_sum(cam_shake_dvec,vec_mul(cam_shake_vel,time));
end;

procedure add_cam_shake(val:double;dir:Tvec);
begin
if val>0 then
	begin
	cam_shake_vel:=vec_sum(cam_shake_vel,vec_mul(dir,val*CAMERA_SHAKING_POWER));
	cam_shake:=true;
  end;
end;