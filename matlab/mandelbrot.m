%%
scut = -0.8+0.156i;%(0.25-1.1j);
blin = 1;
n_iter = 20;
bjulia = 1;

%%
if blin
    [gx, gy] = meshgrid([-2:1/256:2], [-2:1/256:2]);
    z0 = gx + 1j*gy;
else
    nn = 1e7;
    z0 = 4*rand([nn,1])-2 + 1j*(4*rand([nn,1])-2);
end

if bjulia,
   zz = z0;
   z0 = scut * ones(size(zz));
else
    zz = scut * ones(size(z0));
end

%%
for ii=1:n_iter,
    zz=zz.^2 + z0;
    fprintf('%d/%d\n', ii, n_iter);
end

%%
rad = 2;
pts = zz(abs(zz) < rad);
pix = 1024;
rr = linspace(-rad, rad, pix);
n = hist3([imag(pts), real(pts)], [pix, pix]);
imagesc(rr, rr, log(n))
colormap(hot)
axis square

%%
vv=log(n);
imwrite(64*vv/max(vv(:)), hot, 'mandelbrot.png')