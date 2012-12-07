constants;
mod_width = 2;
mod_height = 2;

num_mod = 400 / (mod_width * mod_height);

output = sim(net, trainDB(:,1:TRAINING_SET));

disp(sprintf('Forming the modified input sets...'));
modDB = [];
count = 0;
final_out = [];
for i=1:mod_height:(20-mod_height+1)
    for j=1:mod_width:(20-mod_width+1)
        count = count + 1;
        disp(sprintf('Forming the %ith modified input sets (%i,%i)...', count, i, j));
        modi = ((i-1)/mod_width)+1;
        modj = ((j-1)/mod_height)+1;
        disp(sprintf('modi = %i, modj = %i...', modi, modj));
        modDB{modi, modj} = trainDB;
        for k=1:mod_width
            for l=1:mod_height
                for m=1:TRAINING_SET
                    modDB{modi, modj}((20*(i+l-2) + j + k - 1) ,m) = randInt(1,1,256);
                end
            end
        end
        outDB = sim(net, modDB{modi,modj}(:,1:TRAINING_SET));
        out_del = outDB - output;
        sum_squares = sum (out_del.*out_del);
        final_out(modi,modj)=[sqrt(sum_squares/TRAINING_SET)];
    end
end

colormap bone;
surf(medfilt2(interp2(final_out,2),[4 4]));


                
                

    
    
    
    

            