h = 0.1;
hf = 0.025;
Point(1) = {-1, -1, 0, h};
Point(2) = {1, -1, 0, h};
Point(3) = {1, 0, 0, hf};
Point(4) = {1, 1, 0, h};
Point(5) = {-1, 1, 0, h};
Point(6) = {-1, 0, 0, hf};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 1};
Line(7) = {3, 6};
Line Loop(9) = {7, -5, -4, -3};
Plane Surface(9) = {9};
Line Loop(11) = {6, 1, 2, 7};
Plane Surface(11) = {11};
