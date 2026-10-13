library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity bench is
    port
    (
        clk_i         : in  std_logic;
        start_i       : in  std_logic;
        reset_i       : in  std_logic;
        sel_i         : in  std_logic_vector(2 downto 0); -- SW(2..0) Selector de prueba
        primos_o      : out std_logic_vector(13 downto 0);
        finish_o      : out std_logic;

        decenas_seg_o : out std_logic_vector(2 downto 0);

        HEX0_O        : out std_logic_vector(6 downto 0);
        HEX1_O        : out std_logic_vector(6 downto 0);
        HEX2_O        : out std_logic_vector(6 downto 0);
        HEX3_O        : out std_logic_vector(6 downto 0)
    );
end bench;

architecture Behavioral of bench is

    type state_type is (
        S_IDLE, 
        S_ADD, 
        S_MUL, 
        S_DIV, 
        S_INIT_D, S_SUBTRACT, S_EVALUATE, S_NEXT_N, -- Evaluación de Primos
        S_RLE,                                      -- Compresión RLE
        S_FINISH
    );
    signal state : state_type := S_IDLE;

    -- Registros generales
    signal counter      : unsigned(31 downto 0) := (others => '0');
    signal iteraciones  : unsigned(31 downto 0) := (others => '0');
    signal acc          : unsigned(31 downto 0) := (others => '0'); -- Acumulador (Anti Dead-Code Elimination)

    -- Registros para Multiplicación y División
    signal mul_b  : unsigned(31 downto 0) := to_unsigned(12345, 32);
    signal div_q  : unsigned(31 downto 0) := to_unsigned(1, 32);
    signal div_r  : unsigned(31 downto 0) := (others => '0');
    signal div_k  : unsigned(5 downto 0)  := (others => '0');
    constant K_DIV: unsigned(31 downto 0) := to_unsigned(7, 32);

    -- Registros para Primos
    signal N            : unsigned(15 downto 0) := to_unsigned(2, 16);
    signal D            : unsigned(15 downto 0) := to_unsigned(2, 16);
    signal remainder    : unsigned(15 downto 0) := (others => '0');
    signal count_primus : unsigned(13 downto 0) := (others => '0');

    -- Registros para Compresión RLE
    type rom_array is array (0 to 15) of unsigned(7 downto 0);
    constant DATA_ROM : rom_array := (
        x"41", x"41", x"41", x"41", 
        x"42", x"42",               
        x"43",                      
        x"44", x"44", x"44", x"44", 
        x"45", x"45", x"45", x"45", 
        x"46"                       
    );
    signal rle_idx  : integer range 0 to 15 := 0;
    signal rle_run  : unsigned(7 downto 0)  := to_unsigned(1, 8);
    signal rle_pass : unsigned(15 downto 0) := (others => '0');

    -- Señales para el cronómetro
    signal cnt_50k      : unsigned(15 downto 0) := (others => '0');
    signal bcd0, bcd1, bcd2, bcd3 : unsigned(3 downto 0) := (others => '0');
    signal overflow_cnt : unsigned(2 downto 0) := (others => '0');

    -- Decodificador BCD a 7 segmentos
    function bcd_to_7seg(bcd : unsigned(3 downto 0)) return std_logic_vector is
        variable seg : std_logic_vector(6 downto 0);
    begin
        case bcd is
            when "0000" => seg := "1000000"; -- 0
            when "0001" => seg := "1111001"; -- 1
            when "0010" => seg := "0100100"; -- 2
            when "0011" => seg := "0110000"; -- 3
            when "0100" => seg := "0011001"; -- 4
            when "0101" => seg := "0010010"; -- 5
            when "0110" => seg := "0000010"; -- 6
            when "0111" => seg := "1111000"; -- 7
            when "1000" => seg := "0000000"; -- 8
            when "1001" => seg := "0010000"; -- 9
            when others => seg := "1111111"; -- Apagado
        end case;
        return seg;
    end function;

begin

    -- ===============================================
    -- PROCESO 1: Máquina de estados (Benchmark)
    -- ===============================================
    process (clk_i, reset_i)
        variable prod : unsigned(63 downto 0);
        variable r_n  : unsigned(31 downto 0);
        variable q_n  : unsigned(31 downto 0);
    begin
        if reset_i = '1' then
            state        <= S_IDLE;
            counter      <= (others => '0');
            iteraciones  <= (others => '0');
            acc          <= (others => '0');
            mul_b        <= to_unsigned(12345, 32);
            div_q        <= to_unsigned(1, 32);
            div_r        <= (others => '0');
            div_k        <= (others => '0');
            N            <= to_unsigned(2, 16);
            D            <= to_unsigned(2, 16);
            remainder    <= (others => '0');
            count_primus <= (others => '0');
            rle_idx      <= 0;
            rle_run      <= to_unsigned(1, 8);
            rle_pass     <= (others => '0');
            finish_o     <= '0';

        elsif rising_edge(clk_i) then
            case state is

                when S_IDLE =>
                    finish_o     <= '0';
                    counter      <= (others => '0');
                    iteraciones  <= to_unsigned(1, 32);
                    acc          <= (others => '0');
                    mul_b        <= to_unsigned(12345, 32);
                    div_q        <= to_unsigned(1, 32);
                    div_r        <= (others => '0');
                    div_k        <= (others => '0');
                    N            <= to_unsigned(2, 16);
                    D            <= to_unsigned(2, 16);
                    count_primus <= (others => '0');
                    rle_idx      <= 0;
                    rle_run      <= to_unsigned(1, 8);
                    rle_pass     <= (others => '0');

                    if start_i = '1' then
                        case sel_i is
                            when "000"  => state <= S_ADD;
                            when "001"  => state <= S_MUL;
                            when "010"  => state <= S_DIV;
                            when "011"  => state <= S_INIT_D; -- Primos
                            when others => state <= S_RLE;    -- RLE
                        end case;
                    end if;

                ----------------------------------------------------------------
                -- 1. SUMA DE ENTEROS (1,000,000 operaciones)
                ----------------------------------------------------------------
                when S_ADD =>
                    counter <= counter + 1;
                    acc     <= acc + iteraciones;
                    if iteraciones = 1000000 then
                        state <= S_FINISH;
                    else
                        iteraciones <= iteraciones + 1;
                    end if;

                ----------------------------------------------------------------
                -- 2. MULTIPLICACIÓN DE ENTEROS (1,000,000 operaciones)
                ----------------------------------------------------------------
                when S_MUL =>
                    prod    := iteraciones * mul_b;
                    counter <= counter + 1;
                    acc     <= acc xor prod(31 downto 0);
                    mul_b   <= mul_b + 3;
                    if iteraciones = 1000000 then
                        state <= S_FINISH;
                    else
                        iteraciones <= iteraciones + 1;
                    end if;

                ----------------------------------------------------------------
                -- 3. DIVISIÓN DE ENTEROS (1,000,000 operaciones, 32 ciclos/div)
                ----------------------------------------------------------------
                when S_DIV =>
                    r_n := div_r(30 downto 0) & div_q(31);
                    q_n := div_q(30 downto 0) & '0';
                    if r_n >= K_DIV then
                        r_n := r_n - K_DIV;
                        q_n(0) := '1';
                    end if;
                    div_r   <= r_n;
                    div_q   <= q_n;
                    counter <= counter + 1;
                    div_k   <= div_k + 1;

                    if div_k = 31 then
                        acc   <= acc + q_n;
                        div_k <= (others => '0');
                        div_r <= (others => '0');
                        if iteraciones = 1000000 then
                            state <= S_FINISH;
                        else
                            iteraciones <= iteraciones + 1;
                            div_q       <= iteraciones + 1;
                        end if;
                    end if;

                ----------------------------------------------------------------
                -- 4. PRIMOS (Evaluación de 2 a 60,000)
                ----------------------------------------------------------------
                when S_INIT_D =>
                    D         <= to_unsigned(2, 16);
                    remainder <= N;
                    counter   <= counter + 1;
                    state     <= S_SUBTRACT;

                when S_SUBTRACT =>
                    counter <= counter + 1;
                    if remainder >= D then
                        remainder <= remainder - D;
                    else
                        state <= S_EVALUATE;
                    end if;

                when S_EVALUATE =>
                    counter <= counter + 1;
                    if remainder = 0 then
                        if N = D then
                            count_primus <= count_primus + 1;
                        end if;
                        state <= S_NEXT_N;
                    else
                        if (D * D) >= N then
                            count_primus <= count_primus + 1;
                            state <= S_NEXT_N;
                        else
                            D <= D + 1;
                            remainder <= N;
                            state <= S_SUBTRACT;
                        end if;
                    end if;

                when S_NEXT_N =>
                    counter <= counter + 1;
                    if N = to_unsigned(60000, 16) then
                        state <= S_FINISH;
                    else
                        N <= N + 1;
                        state <= S_INIT_D;
                    end if;

                ----------------------------------------------------------------
                -- 5. COMPRESIÓN RLE (10,000 pasadas sobre ROM)
                ----------------------------------------------------------------
                when S_RLE =>
                    counter <= counter + 1;
                    if rle_idx < 15 then
                        if DATA_ROM(rle_idx) = DATA_ROM(rle_idx + 1) then
                            rle_run <= rle_run + 1;
                        else
                            acc     <= acc + resize(DATA_ROM(rle_idx) xor rle_run, 32);
                            rle_run <= to_unsigned(1, 8);
                        end if;
                        rle_idx <= rle_idx + 1;
                    else
                        acc     <= acc + resize(DATA_ROM(rle_idx) xor rle_run, 32);
                        rle_idx <= 0;
                        rle_run <= to_unsigned(1, 8);
                        if rle_pass >= 10000 then
                            state <= S_FINISH;
                        else
                            rle_pass <= rle_pass + 1;
                        end if;
                    end if;

                ----------------------------------------------------------------
                -- ESTADO FINAL
                ----------------------------------------------------------------
                when S_FINISH =>
                    finish_o <= '1';

            end case;
        end if;
    end process;

    -- ===============================================
    -- PROCESO 2: Cronómetro (Mide tiempo en tiempo real)
    -- ===============================================
    process (clk_i, reset_i)
    begin
        if reset_i = '1' then
            cnt_50k      <= (others => '0');
            bcd0         <= (others => '0');
            bcd1         <= (others => '0');
            bcd2         <= (others => '0');
            bcd3         <= (others => '0');
            overflow_cnt <= (others => '0');

        elsif rising_edge(clk_i) then
            -- Reiniciamos el cronómetro cuando estamos en IDLE
            if state = S_IDLE then
                cnt_50k      <= (others => '0');
                bcd0         <= (others => '0');
                bcd1         <= (others => '0');
                bcd2         <= (others => '0');
                bcd3         <= (others => '0');
                overflow_cnt <= (others => '0');

            -- Solo contamos mientras esté calculando
            elsif state /= S_FINISH then

                if cnt_50k = 49999 then
                    cnt_50k <= (others => '0'); -- Pasó 1 milisegundo

                    -- Lógica de incremento BCD
                    if bcd0 = 9 then
                        bcd0 <= (others => '0');
                        if bcd1 = 9 then
                            bcd1 <= (others => '0');
                            if bcd2 = 9 then
                                bcd2 <= (others => '0');
                                if bcd3 = 9 then
                                    bcd3 <= (others => '0');
                                    overflow_cnt <= overflow_cnt + 1;
                                else
                                    bcd3 <= bcd3 + 1;
                                end if;
                            else
                                bcd2 <= bcd2 + 1;
                            end if;
                        else
                            bcd1 <= bcd1 + 1;
                        end if;
                    else
                        bcd0 <= bcd0 + 1;
                    end if;

                else
                    cnt_50k <= cnt_50k + 1;
                end if;

            end if;
        end if;
    end process;

    -- ===============================================
    -- ASIGNACIÓN DE SALIDAS FÍSICAS
    -- ===============================================
    -- Muestra la cantidad de primos en la prueba de primos, o la suma checksum en otras pruebas para evitar Dead-Code Elimination
    primos_o <= std_logic_vector(count_primus) when sel_i = "011" else std_logic_vector(acc(13 downto 0));
    decenas_seg_o <= std_logic_vector(overflow_cnt);

    HEX0_O <= bcd_to_7seg(bcd0); -- Unidades de ms
    HEX1_O <= bcd_to_7seg(bcd1); -- Decenas de ms
    HEX2_O <= bcd_to_7seg(bcd2); -- Centenas de ms
    HEX3_O <= bcd_to_7seg(bcd3); -- Miles de ms (Segundos)

end Behavioral;