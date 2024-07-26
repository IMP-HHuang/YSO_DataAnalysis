# YSO_Data Analysis

  - 1. [class](./class)
      - map.txt
        - det = 0 : YSO
          - cid = 0 : A
          - cid = 1 : B
          - cid = 2 : C
          - cid = 3 : D
        - det = 1 : Ge
        - det = 2 : Pulse
          
     - Raw of YSO

        $$Raw = QDC[2] (5 mus) + QDC[3] (5 \mus) - QDC[0](5 \mus) - QDC1 (5\mus)$$

     - Position for YSO
       
       $$x = \frac{RawA + RawB}{RawA + RawB + RawC + RawD}$$

       $$y = \frac{RawA + RawD}{RawA + RawB + RawC + RawD}$$
