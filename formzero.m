function mattrans=formzero(Numer, Denumer)
% Function mattrans=formzero(Numer, Denumer)
% Calculating transfer matrix from numerator and denumerator polynoms matrix with dt=0
% IN:
% Numer - numerators' coefficient matrix
% Denumer - denumerators' coefficient matrix

for i=1:size(Numer,1)
    for k=1:size(Numer,2)
        mattrans(i,k)=Numer{i,k}(length(Numer{i,k}))/Denumer{i,k}(length(Denumer{i,k}));
    end
end