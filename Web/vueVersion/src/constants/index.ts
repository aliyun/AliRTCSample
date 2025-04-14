export const colors = [
  'rgba(224, 44, 11, 1)',
  'rgba(220, 0, 129, 1)',
  'rgba(228, 100, 8, 1)',
  'rgba(231, 189, 14, 1)',
  'rgba(86, 19, 216, 1)',
  'rgba(22, 49, 211, 1)',
  'rgba(12, 142, 229, 1)',
  'rgba(32, 216, 8, 1)',
  'rgba(144, 222, 15, 1)',
  'rgba(1, 2, 4, 1)',
  'rgba(153, 153, 153, 1)',
  // 'rgba(255, 255, 255, 1)',
];

export function randomWbColor() {
  const res = colors[Math.floor(Math.random() * colors.length)];
  return res;
}

export const defaultWhiteboardId = 'default';
