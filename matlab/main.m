constants;
iter=1;
lastSave=1;
while(iter<=NO_TRAINING_ITERATIONS)
    if (mod(iter,1)==0 && lastSave~=iter)
        disp ('Saving network...');
        file1=['../netnew/t5/net/network_' sprintf('%i',cnt)];
        save(file1,'net');
        saveNetwork(net,sprintf('../netnew/t5/weights/T%iL1.txt',cnt),sprintf('../netnew/t5/weights/T%iL2.txt',cnt));
        cnt=cnt+1;
        lastSave=iter;
    end
    disp(sprintf('----------PERFORMING TRAINING NO %i------------',iter));
    trainer2;
    tester;
end
disp(sprintf('----------TRAINING COMPLETE!------------',i));

