%The main macros to simulate thyristors shema version 2.4
%IN:
%Tanalys - analys time
%dt - time step
%d0 - max commutation time drawback
%S - level of razvazka
%Topology - shema topology matrix
%EJm - E and J matrix
%IN1 - U and I vector of C and L (optional)
%Kvs - thyristors open times matrix
%Nd - number of discretes in thyristors' control period (optional)
%kmc - matrix of magnetic connections
%OUT:
%rez - matrix with results

%profile on
tic
%modeling...
steps=Tanalys/dt;
rez=[]; t=[]; dnod=[]; inod=[];
trans=[]; graph=[]; vinx=[]; vunx=[];
topo=[]; iner=[]; ej=[]; IN=[];
Num=[]; Den=[];
t=zeros(1,round(steps));
n=eval('kmc','[]');
kmc=n;
[topo,branch,connect,Mmc]=MakeTree(Topology,S,kmc);
graph=MakeSigGraph(topo,branch,connect,Mmc);
[dnod,inod,iner,vinx,vunx]=FindDInodes(topo);
IN=eval('IN1','zeros(size(iner,1),1)');
n=eval('Nd','steps');
Nd=n;
ej=computeej(EJm,0);
[Num,Den]=make3mat(graph,dnod,vertcat(inod,iner(:,1)));
trans=formzero(Num,Den);
rez=zeros(length(dnod),round(steps));
rez(:,1)=trans*vertcat(ej,IN);
trans=formmat(Num,Den,dt);
for n=1:steps
    t(n+1)=dt*n;
    ej=computeej(EJm,t(n+1));
    rez(:,n+1)=trans*vertcat(ej,IN);
    %Look for closing thyristors
    if ~isempty(find(rez(vinx,n+1)<0))
        a=0; b=dt;
        time=t(n);
        fa=rez(:,n);
        fb=rez(:,n+1);
      while ~isempty(find(fb(vinx)<0))
        max1=1+round((log(b-a)-log(d0))/log(2));
        for k=1:max1
            c=(a+b)/2;
            ej=computeej(EJm,time+c);
            fc=formmat(Num,Den,c-a)*vertcat(ej,IN);
            if ~isempty(find(fc(vinx)<0))
               b=c;
               fb=fc;
           else
               a=c;
               fa=fc;
               IN=fa(iner(:,2));
           end
           if b-a<d0, break,end
       end
       c=(a+b)/2;
       ej=computeej(EJm,time+c);
       fc=formmat(Num,Den,c-a)*vertcat(ej,IN);
       for k=1:length(vinx)
           if fb(vinx(k))<0
               Topology((dnod(vinx(k))+1)/3,1)=5;
           end
       end
       [topo,branch,connect]=MakeTree(Topology,S,kmc);
       graph=MakeSigGraph(topo,branch,connect,Mmc);
       time=time+c;
       a=0;
       fa=fc;
       ej=computeej(EJm,time);
       IN=fa(iner(:,2));
       [Num,Den]=make3mat(graph,dnod,vertcat(inod,iner(:,1)));
       fc=formzero(Num,Den)*vertcat(ej,IN);
       if ~isempty(find(fc(vinx)<0))
        for k=1:length(vinx)
            if fc(vinx(k))<0
                Topology((dnod(vinx(k))+1)/3,1)=5;
            end
        end
        [topo,branch,connect]=MakeTree(Topology,S,kmc);
        graph=MakeSigGraph(topo,branch,connect,Mmc);
        [Num,Den]=make3mat(graph,dnod,vertcat(inod,iner(:,1)));
        fc=formzero(Num,Den)*vertcat(ej,IN);
       end
       fa=fc;b=t(n)+dt-time;
       IN=fa(iner(:,2));
       ej=computeej(EJm,time+b);
       fb=formmat(Num,Den,b)*vertcat(ej,IN);
      end
       rez(:,n+1)=fb;
       trans=formmat(Num,Den,dt);
    end
    chg=logical(0);
    ch_gl=logical(0);
    %Looking for opening thyristors
    for k=1:length(vunx)
        if (rez(vunx(k),n+1)>0) & (Kvs(k)==mod(n+1,Nd))
            ch_gl=logical(1);
            Topology(round(dnod(vinx(k))/3),1)=3;
        end
    end
    if ch_gl
        [topo,branch,connect]=MakeTree(Topology,S,kmc);
        graph=MakeSigGraph(topo,branch,connect,Mmc);
        fb=MakeTMat(graph,dnod,vertcat(inod,iner(:,1)),dt)*vertcat(ej,IN);
        IN=fb(iner(:,2));
        for k=1:length(vinx)
            if fb(vinx(k))<0
                chg=logical(1);
                Topology((dnod(vinx(k))+1)/3,1)=5;
            end
        end
        while chg
            chg=logical(0);
            [topo,branch,connect]=MakeTree(Topology,S,kmc);
            graph=MakeSigGraph(topo,branch,connect,Mmc);
            fb=MakeTMat(graph,dnod,vertcat(inod,iner(:,1)),0)*vertcat(ej,IN);
            for k=1:length(vinx)
                if fb(vinx(k))<0
                    chg=logical(1);
                    Topology((dnod(vinx(k))+1)/3,1)=5;
                end
            end
        end
        rez(:,n+1)=fb;
        [Num,Den]=make3mat(graph,dnod,vertcat(inod,iner(:,1)));
        trans=formmat(Num,Den,dt);
    end
    IN=rez(iner(:,2),n+1);
end
time=toc;
disp('Computing complete in');
disp(time); disp('seconds');
msgbox('Calculation complete.','MMATS')
%profile report