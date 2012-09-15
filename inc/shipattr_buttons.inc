a:=calc_shipattr_cost(ship_type[plship^.type_]^.life);
  if a>=money then
		begin
    shipattr1but^.state:=BTS_DISABLED;
  	gf_coladd.r:=120;
  	with gf_colsub do begin r:=40;g:=30;b:=50;end;
  	end else
  if shipattr1but^.state=BTS_DISABLED then shipattr1but^.state:=BTS_NORMAL;
  destroyimage(shipattr1but^.img);
  shipattr1but^.img:=gf_text_img(to_str(a),font4);
  shipattr1but^.w:=getimagewidth(shipattr1but^.img);
  shipattr1but^.h:=getimageheight(shipattr1but^.img);
  shipattr1but^.press_img:=shipattr1but^.img;
  gf_reset_colors;

a:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.move_speed*100));
  if a>=money then
		begin
    shipattr2but^.state:=BTS_DISABLED;
  	gf_coladd.r:=120;
  	with gf_colsub do begin r:=40;g:=30;b:=50;end;
  	end else
  if shipattr2but^.state=BTS_DISABLED then shipattr2but^.state:=BTS_NORMAL;
  destroyimage(shipattr2but^.img);
  shipattr2but^.img:=gf_text_img(to_str(a),font4);
  shipattr2but^.w:=getimagewidth(shipattr2but^.img);
  shipattr2but^.h:=getimageheight(shipattr2but^.img);
  shipattr2but^.press_img:=shipattr2but^.img;
  gf_reset_colors;

a:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.bullet_dmg*100));
  if a>=money then
		begin
    shipattr3but^.state:=BTS_DISABLED;
  	gf_coladd.r:=120;
  	with gf_colsub do begin r:=40;g:=30;b:=50;end;
  	end else
  if shipattr3but^.state=BTS_DISABLED then shipattr3but^.state:=BTS_NORMAL;
  destroyimage(shipattr3but^.img);
  shipattr3but^.img:=gf_text_img(to_str(a),font4);
  shipattr3but^.w:=getimagewidth(shipattr3but^.img);
  shipattr3but^.h:=getimageheight(shipattr3but^.img);
  shipattr3but^.press_img:=shipattr3but^.img;
  gf_reset_colors;

a:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.bullet_speed*100));
  if a>=money then
		begin
    shipattr4but^.state:=BTS_DISABLED;
  	gf_coladd.r:=120;
  	with gf_colsub do begin r:=40;g:=30;b:=50;end;
  	end else
  if shipattr4but^.state=BTS_DISABLED then shipattr4but^.state:=BTS_NORMAL;
  destroyimage(shipattr4but^.img);
  shipattr4but^.img:=gf_text_img(to_str(a),font4);
  shipattr4but^.w:=getimagewidth(shipattr4but^.img);
  shipattr4but^.h:=getimageheight(shipattr4but^.img);
  shipattr4but^.press_img:=shipattr4but^.img;
  gf_reset_colors;

a:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.attack_speed*100));
  if a>=money then
		begin
    shipattr5but^.state:=BTS_DISABLED;
  	gf_coladd.r:=120;
  	with gf_colsub do begin r:=40;g:=30;b:=50;end;
  	end else
  if shipattr5but^.state=BTS_DISABLED then shipattr5but^.state:=BTS_NORMAL;
  destroyimage(shipattr5but^.img);
  shipattr5but^.img:=gf_text_img(to_str(a),font4);
  shipattr5but^.w:=getimagewidth(shipattr5but^.img);
  shipattr5but^.h:=getimageheight(shipattr5but^.img);
  shipattr5but^.press_img:=shipattr5but^.img;
  gf_reset_colors;
