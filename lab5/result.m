%compare with actual results

load testSet.mat;
load NN.mat;

counter = 0;

for i = 1:10000
   a = neural(i,testData,finalW1L1,finalB1L1,finalB1L2,finalW1L2,finalSoftmaxTheta);
   if a == testLabels(i)
       counter = counter + 1;
   end
end

counter