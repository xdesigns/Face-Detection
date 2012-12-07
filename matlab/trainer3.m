disp('Commencing Training Process...');
if (DO_VALIDATION)
    net=train(net,trainDB,targetOutput,[],[],validation);
else
    net=train(net,trainDB,targetOutput);
end