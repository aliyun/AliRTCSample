import React from 'react';

export const NetworkScore: React.FC<{ className?: string; signalNumber: number }> = ({
  className,
  signalNumber,
}) => {
  return (
    <svg
      className={className || ''}
      viewBox="0 0 1024 1024"
      version="1.1"
      xmlns="http://www.w3.org/2000/svg"
      id="mx_n_1710492163232"
      data-spm-anchor-id="a313x.manage_type_myprojects.0.i9.27f83a81kRjPNe"
    >
      <path
        d="M176 512m64 0l32 0q64 0 64 64l0 128q0 64-64 64l-32 0q-64 0-64-64l0-128q0-64 64-64Z"
        data-spm-anchor-id="a313x.manage_type_myprojects.0.i7.27f83a81kRjPNe"
        className="selected"
        fill={signalNumber > 0 ? 'rgba(28,163,61,0.9)' : ''}
      />
      <path
        d="M432 384m64 0l32 0q64 0 64 64l0 256q0 64-64 64l-32 0q-64 0-64-64l0-256q0-64 64-64Z"
        data-spm-anchor-id="a313x.manage_type_myprojects.0.i6.27f83a81kRjPNe"
        className="selected"
        fill={signalNumber > 1 ? 'rgba(28,163,61,0.9)' : ''}
      />
      <path
        d="M688 256m64 0l32 0q64 0 64 64l0 384q0 64-64 64l-32 0q-64 0-64-64l0-384q0-64 64-64Z"
        data-spm-anchor-id="a313x.manage_type_myprojects.0.i8.27f83a81kRjPNe"
        fill={signalNumber > 2 ? 'rgba(28,163,61,0.9)' : ''}
      />
    </svg>
  );
};
