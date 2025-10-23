import { Popover } from 'ant-design-vue';
import { LanguageInfo } from 'dingrtc-asr';
import Icon from '~/components/Icon';
import { computed, defineComponent } from 'vue';
import styles from './index.module.less';
import { useAsrInfo } from '~/store';
import { useAsrHooks } from '~/hooks/channel';

export const ChangeTranslatePanel = defineComponent(() => {
  const asrInfo = useAsrInfo();

  const { changeTranslateLanguages, changeDualLang } = useAsrHooks();

  const translateLanguages = computed(() => asrInfo.asr.supportedTranslateLanguages);

  const dualLang = {
    language: 'source',
    displayName: '同时显示双语',
  }

  return () => (
    <div class={styles.translatePanel}>
      <TranslateChooseBox
        langList={
          translateLanguages.value.length > 5
            ? [...translateLanguages.value.slice(0, 5), { displayName: '更多语言', language: 'other' }]
            : translateLanguages.value
        }
        title={'你看的语言'}
        curLang={asrInfo.transLang}
        moreList={translateLanguages.value.slice(5)}
        updateLang={changeTranslateLanguages}
      />
      <div class={styles.langDivider} />
      <LangItem item={dualLang} selected={asrInfo.dualLang} updateLang={() => changeDualLang(!asrInfo.dualLang)}  />
    </div>
  );
});

interface TranslateChooseBoxProps {
  langList: LanguageInfo[];
  title: string;
  curLang: string;
  updateLang: (lang: string) => void;
  moreList?: LanguageInfo[];
}

const TranslateChooseBox = defineComponent((props: TranslateChooseBoxProps) => {
  const { langList, moreList, title, updateLang } = props;
  return () => (
    <>
      <div class={styles.originLangBox}>
        <div class={styles.originLangTitle}>{title}</div>
        {langList?.map((item) => {
          const selected =
            item.language === 'other' && moreList
              ? moreList?.some((lan) => props.curLang === lan.language)
              : props.curLang === item.language;
          if (item.language === 'other') {
            return (
              <Popover
                key={item.language}
                placement="right"
                arrow={false}
                content={
                  <div class={styles.translatePanel}>
                    <TranslateChooseBox
                      langList={moreList || []}
                      title={''}
                      curLang={props.curLang}
                      updateLang={updateLang}
                    />
                  </div>
                }
                trigger="hover"
              >
                <div>
                  <LangItem item={item} updateLang={updateLang} selected={selected} />
                </div>
              </Popover>
            );
          }
          return (
            <LangItem key={item.language} item={item} updateLang={updateLang} selected={selected} />
          );
        })}
      </div>
    </>
  );
}, { props: ['curLang', 'langList', 'moreList', 'title', 'updateLang'] });

interface LangItemProps {
  item: LanguageInfo;
  updateLang: (lang: string) => void;
  selected: boolean;
}

const LangItem = defineComponent((props: LangItemProps) => {
  const { item, updateLang } = props;
  return () => (
    <div
      class={styles.langItemWrapper}
      key={item.language}
      onClick={() => {
        if (item.language !== 'other') {
          updateLang(item.language);
        }
      }}
    >
      <div class={styles.checkBox}>
        <div style={{ opacity: props.selected ? 1 : 0 }}>
          <Icon type="icon-check-thin" />
        </div>
      </div>
      <div class={styles.langItem}>
        <div class={styles.moreInfo}>
          <span>{item.displayName}</span>
        </div>
        {item.language === 'other' ? <Icon type="icon-rightArrow" /> : null}
      </div>
    </div>
  );
}, { props: ['item', 'selected', 'updateLang'] });
