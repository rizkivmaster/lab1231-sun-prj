function transform()
    scale=0.2;
    sourcepngfolder='/home/rizkivmaster/ws/datasets/voc2010/SegmentationClass';
    sourcejpgfolder='/home/rizkivmaster/ws/datasets/voc2010/JPEGImages';
    targetpngfolder='/home/rizkivmaster/ws/datasets/miniVOC2010/SegmentationClass';
    targetjpgfolder='/home/rizkivmaster/ws/datasets/miniVOC2010/JPEGImages';
    
    listpng = dir([sourcepngfolder '/*.png']);
    for ll = 1:size(listpng,1)
        
        [pathstr,name,ext] = fileparts(listpng(ll).name)
        
        %set out path
        sourcepngpath = [sourcepngfolder '/' name '.png'];
        sourcejpgpath = [sourcejpgfolder '/' name '.jpg'];
        targetpngpath = [targetpngfolder '/' name '.png'];
        targetjpgpath = [targetjpgfolder '/' name '.jpg'];
        
        %read data
        [sourcepng pallette] = imread(sourcepngpath);
        sourcejpg = imread(sourcejpgpath);
        
        %convert and save png
        targetwidth = floor(size(sourcepng,2)*scale);
        targetheight = floor(size(sourcepng,1)*scale);
        targetpng=zeros(targetheight,targetwidth);
        for ii = 1:targetheight
            for jj = 1:targetwidth
		          iimama = floor(ii/scale);
                jjmama = floor(jj/scale);
                if(sourcepng(iimama,jjmama)==255)
                    targetpng(ii,jj) = 256;
                else
                    targetpng(ii,jj) = int32(sourcepng(iimama,jjmama)+1);
                end            
	    end
        end
        
        imwrite(targetpng,pallette,targetpngpath,'png');
        
        %convert and save jpg
        targetjpg = imresize(sourcejpg, [targetheight targetwidth]);
        imwrite(targetjpg, targetjpgpath,'jpg');
end
% end
