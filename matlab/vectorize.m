function vec=vectorize(A)
    dims=size(A);
    vec=[];
    for i=1:dims(1)
        vec=[vec A(i,:)];
    end
end